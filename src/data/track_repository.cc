// track_repository.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <libaudcore/runtime.h>

#include "infrastructure/event/delegate.h"
#include "data/provider_type.h"
#include "data/providers/ampache.h"
#include "data/providers/cache.h"
#include "data_objects/track_data.h"
#include "data_objects/album_data.h"
#include "data/filters/filter.h"
#include "data/indices.h"
#include "data/artist_repository.h"
#include "data/album_repository.h"
#include "data/track_repository.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

TrackRepository::TrackRepository(Ampache& ampache, Cache& cache, const ArtistRepository& artistRepository,
    const AlbumRepository& albumRepository, Indices& indices):
myAmpache(ampache),
myCache(cache),
myArtistRepository(artistRepository),
myAlbumRepository(albumRepository),
myIndices(indices) {
    myUnfilteredFilter->setSourceData(myTracksData);
    myUnfilteredFilter->changed += DELEGATE0(&TrackRepository::onFilterChanged);
    myFilter = myUnfilteredFilter;

    myAmpache.readyTracks += DELEGATE1(&TrackRepository::onReadyTracks, vector<unique_ptr<TrackData>>);
}



TrackRepository::~TrackRepository() {
    myAmpache.readyTracks -= DELEGATE1(&TrackRepository::onReadyTracks, vector<unique_ptr<TrackData>>);
    myUnfilteredFilter->changed -= DELEGATE0(&TrackRepository::onFilterChanged);
    if (isFiltered()) {
        myFilter->changed -= DELEGATE0(&TrackRepository::onFilterChanged);
    }
}



void TrackRepository::setProviderType(ProviderType providerType) {
    if (myProviderType != providerType) {
        myProviderType = providerType;
        clear();

        providerChanged();

        if (maxCount() == 0) {
            auto error = false;
            fullyLoaded(error);
        }
    }
}



/**
 * @warning Class does not work correctly if this method is called multiple times for the same data.
 */
bool TrackRepository::load(int offset, int limit) {
    if (myLoadOffset != -1 || !myLoadingEnabled) {
        return false;
    }

    AUDDBG("Load from %d, limit %d.\n", offset, limit);
    if (myProviderType == ProviderType::Ampache) {
        myLoadOffset = offset;
        myAmpache.requestTracks(offset, limit);
    } else if (myProviderType == ProviderType::Cache) {
        if (myLoadProgress == 0) {
            loadFromCache();
        }
    }
    return true;
}



Track& TrackRepository::get(int filteredOffset) const {
    TrackData& trackData = myFilter->getFilteredData()[filteredOffset];
    return trackData.getTrack();
}



Track& TrackRepository::getById(const string& id) const {
    auto tracksDataIter = find_if(myTracksData.begin(), myTracksData.end(),
        [&id](const unique_ptr<TrackData>& td) {return td->getId() == id;});
    return (*tracksDataIter)->getTrack();
}



bool TrackRepository::isLoaded(int filteredOffset, int count) const {
    uint end = filteredOffset + count;
    auto filteredTracksData = myFilter->getFilteredData();
    return (filteredTracksData.size() >= end) && all_of(filteredTracksData.begin() + filteredOffset,
        filteredTracksData.begin() + filteredOffset + count, [](const TrackData& td) {return &td != nullptr;});
}



int TrackRepository::maxCount() {
    if (myCachedMaxCount == -1) {
        myCachedMaxCount = computeMaxCount();
    }
    return myCachedMaxCount;
}



void TrackRepository::disableLoading() {
    myLoadingEnabled = false;
    myCachedMaxCount = -1;
    loadingDisabled();
    if (myLoadOffset == -1) {
        auto error = false;
        fullyLoaded(error);
    }
}



void TrackRepository::setFilter(unique_ptr<Filter<TrackData>> filter) {
    AUDDBG("Setting a filter.\n");
    myIsFilterSet = true;

    myFilter->changed -= DELEGATE0(&TrackRepository::onFilterChanged);

    filter->setSourceData(myTracksData);
    filter->changed += DELEGATE0(&TrackRepository::onFilterChanged);
    myFilter = move(filter);
    myFilter->apply();
}



void TrackRepository::unsetFilter() {
    if (!isFiltered()) {
        return;
    }
    AUDDBG("Unsetting a filter.\n");
    myIsFilterSet = false;

    myFilter->changed -= DELEGATE0(&TrackRepository::onFilterChanged);

    myUnfilteredFilter->changed += DELEGATE0(&TrackRepository::onFilterChanged);
    myFilter = myUnfilteredFilter;
    myCachedMaxCount = -1;

    filterChanged();
}



bool TrackRepository::isFiltered() const {
    return myIsFilterSet;
}



void TrackRepository::onReadyTracks(vector<unique_ptr<TrackData>>& tracksData) {
    AUDDBG("Ready %d entries from offset %d.\n", tracksData.size(), myLoadOffset);
    bool error = false;

    // return an empty result if the loaded data are not valid anymore (e. g. due to a provider change)
    if (myLoadOffset == -1) {

        // fire loaded event to give a chance to consumers to continue their processing; even in the case of provider
        // change it might not be necessary since consumers should react on providerChanged event by cancelling
        // of all requests
        auto offsetAndLimit = pair<int, int>{0, 0};
        loaded(offsetAndLimit);

        return;
    }

    if (tracksData.size() == 0) {
        error = true;
        fullyLoaded(error);
        return;
    }

    uint offset = myLoadOffset;
    auto end = offset + tracksData.size();
    if (end > myTracksData.size()) {
        myTracksData.resize(end);
    }

    for (auto& trackData: tracksData) {
        auto& track = trackData->getTrack();
        auto& artist = myArtistRepository.getById(trackData->getArtistId());
        track.setArtist(artist);
        auto& album = myAlbumRepository.getById(trackData->getAlbumId());
        track.setAlbum(album);

        updateIndicies(*trackData);

        myTracksData[offset] = move(trackData);
        offset++;
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, tracksData.size()};
    myUnfilteredFilter->processUpdatedSourceData(myLoadOffset, tracksData.size());
    myFilter->apply();
    myLoadOffset = -1;
    myLoadProgress += tracksData.size();
    AUDDBG("Load progress: %d.\n", myLoadProgress);

    bool isFullyLoaded = myLoadProgress >= myAmpache.numberOfTracks();
    if (isFullyLoaded) {
        myCache.saveTracksData(myTracksData);
    }

    // application can be terminated after loaded event therefore there should be no access to instance variables
    // after it is fired
    loaded(offsetAndLimit);
    if (isFullyLoaded || !myLoadingEnabled) {
        fullyLoaded(error);
    }
}



void TrackRepository::onFilterChanged() {
    AUDDBG("Processing filter changed event.\n");
    myCachedMaxCount = -1;

    // to the outside world there is no filter to change if not filtered
    if (isFiltered()) {
        filterChanged();
    }
}



void TrackRepository::clear() {
    AUDDBG("Clearing.\n");
    myTracksData.clear();
    myLoadProgress = 0;
    myLoadOffset = -1;
    myCachedMaxCount = -1;

    myUnfilteredFilter->processUpdatedSourceData(-1, 0);
    myFilter->apply();
    myIndices.clearAlbumsTracks();
    myIndices.clearArtistsAlbums();
    myIndices.clearArtistsTracks();
}



void TrackRepository::loadFromCache() {
    myTracksData = myCache.loadTracksData();

    for (auto& trackData: myTracksData) {
        auto& track = trackData->getTrack();
        auto& artist = myArtistRepository.getById(trackData->getArtistId());
        track.setArtist(artist);
        auto& album = myAlbumRepository.getById(trackData->getAlbumId());
        track.setAlbum(album);

        updateIndicies(*trackData);
    }

    myUnfilteredFilter->processUpdatedSourceData(0, myTracksData.size());
    myFilter->apply();
    myLoadOffset = -1;
    myLoadProgress += myTracksData.size();

    // application can be terminated after loaded event therefore there should be no access to instance variables
    // after it is fired
    auto offsetAndLimit = pair<int, int>{0, myTracksData.size()};
    loaded(offsetAndLimit);
    bool error = false;
    fullyLoaded(error);
}



void TrackRepository::updateIndicies(TrackData& trackData) {
    auto& artist = myArtistRepository.getById(trackData.getArtistId());
    auto& albumData = myAlbumRepository.getAlbumDataById(trackData.getAlbumId());
    myIndices.updateArtistAlbums(artist, albumData);
    myIndices.updateArtistTracks(artist, trackData);
    auto& album = albumData.getAlbum();
    myIndices.updateAlbumTracks(album, trackData);
}



int TrackRepository::computeMaxCount() const {
    if (!myLoadingEnabled || (isFiltered() && myLoadProgress != 0)) {
        return myFilter->getFilteredData().size();
    }
    if (myProviderType == ProviderType::Ampache) {
        return myAmpache.numberOfTracks();
    };
    if (myProviderType == ProviderType::Cache) {
        return myCache.numberOfTracks();
    };
    return 0;
}

}
