// track_repository.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include "infrastructure/event/delegate.h"
#include "data/providers/ampache_service.h"
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

TrackRepository::TrackRepository(AmpacheService& ampacheService, Cache& cache, const ArtistRepository& artistRepository,
    const AlbumRepository& albumRepository, Indices& indices):
myAmpacheService(ampacheService),
myCache(cache),
myArtistRepository(artistRepository),
myAlbumRepository(albumRepository),
myIndices(indices) {
    myUnfilteredFilter->setSourceData(myTracksData);
    myUnfilteredFilter->changed += DELEGATE0(&TrackRepository::onFilterChanged);
    myFilter = myUnfilteredFilter;

    myAmpacheService.readyTracks += DELEGATE1(&TrackRepository::onReadyTracks, vector<unique_ptr<TrackData>>);
}



TrackRepository::~TrackRepository() {
    myAmpacheService.readyTracks -= DELEGATE1(&TrackRepository::onReadyTracks, vector<unique_ptr<TrackData>>);
    myUnfilteredFilter->changed -= DELEGATE0(&TrackRepository::onFilterChanged);
    if (isFiltered()) {
        myFilter->changed -= DELEGATE0(&TrackRepository::onFilterChanged);
    }
}



/**
 * @warning Class does not work correctly if this method is called multiple times for the same data.
 */
bool TrackRepository::load(int offset, int limit) {
    if (myLoadOffset != -1) {
        return false;
    }

    if (!myAmpacheService.getIsConnected() || (myCache.getLastUpdate() > myAmpacheService.getLastUpdate())) {
        if (myLoadProgress == 0) {
            loadFromCache();
        }
    } else {
        myLoadOffset = offset;
        myAmpacheService.requestTracks(offset, limit);
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



void TrackRepository::setFilter(unique_ptr<Filter<TrackData>> filter) {
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

    bool isFullyLoaded = myLoadProgress >= myAmpacheService.numberOfTracks();
    if (isFullyLoaded) {
        myCache.saveTracksData(myTracksData);
    }

    // application can be terminated after loaded event therefore there should be no access to instance variables
    // after it is fired
    loaded(offsetAndLimit);
    if (isFullyLoaded) {
        fullyLoaded();
    }
}



void TrackRepository::onFilterChanged() {
    myCachedMaxCount = -1;

    // to the outside world there is no filter to change if not filtered
    if (isFiltered()) {
        filterChanged();
    }
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
    fullyLoaded();
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
    if (myIsFilterSet && myLoadProgress != 0) {
        return myFilter->getFilteredData().size();
    }
    return myAmpacheService.getIsConnected() ? myAmpacheService.numberOfTracks() : myCache.numberOfTracks();
}

}
