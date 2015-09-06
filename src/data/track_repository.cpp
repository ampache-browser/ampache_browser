// track_repository.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include "data/providers/ampache_service.h"
#include "data/providers/cache.h"
#include "data_objects/track_data.h"
#include "data_objects/album_data.h"
#include "data/artist_repository.h"
#include "data/album_repository.h"
#include "data/track_repository.h"
#include "data/indices.h"

using namespace std;
using namespace placeholders;
using namespace domain;



namespace data {

TrackRepository::TrackRepository(AmpacheService& ampacheService, Cache& cache, ArtistRepository& artistRepository,
    AlbumRepository& albumRepository, Indices& indices):
myAmpacheService(ampacheService),
myCache(cache),
myArtistRepository(artistRepository),
myAlbumRepository(albumRepository),
myIndices(indices) {
    myAmpacheService.readyTracks += bind(&TrackRepository::onReadyTracks, this, _1);
}



/**
 * @warning Class does not work correctly if this method is called multiple times for the same data.
 */
bool TrackRepository::load(int offset, int limit) {
    if (myLoadOffset != -1) {
        return false;
    }

    if (myCache.getLastUpdate() > myAmpacheService.getLastUpdate()) {
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
    TrackData& trackData = myTrackDataReferences[filteredOffset];
    return trackData.getTrack();
}



bool TrackRepository::isLoaded(int filteredOffset, int limit) const {
    uint end = filteredOffset + limit;

    return (myTrackDataReferences.size() >= end) && all_of(myTrackDataReferences.begin() + filteredOffset,
        myTrackDataReferences.begin() + filteredOffset + limit, [](const TrackData& td) {return &td != nullptr;});
}



int TrackRepository::maxCount() {
    if (myCachedMaxCount == -1) {
        myCachedMaxCount = computeMaxCount();
    }
    return myCachedMaxCount;
}



/**
 * @warning May be called no sooner than after the repository is fully loaded.
 */
void TrackRepository::setArtistFilter(vector<reference_wrapper<const Artist>> artists) {
    unsetFilter();
    myIsFilterSet = true;
    myTrackDataReferences.swap(myStoredTrackDataReferences);
    for (auto artist: artists) {
        auto artistIndex = myArtistTrackIndex[artist];
        myTrackDataReferences.insert(myTrackDataReferences.end(), artistIndex.begin(), artistIndex.end());
    }
    myCachedMaxCount = -1;

    bool b = false;
    filterChanged(b);
}



/**
 * @warning May be called no sooner than after the repository is fully loaded.
 */
void TrackRepository::setAlbumFilter(vector<reference_wrapper<const Album>> albums) {
    unsetFilter();
    myIsFilterSet = true;
    myTrackDataReferences.swap(myStoredTrackDataReferences);
    for (auto album: albums) {
        auto albumIndex = myAlbumTrackIndex[album];
        myTrackDataReferences.insert(myTrackDataReferences.end(), albumIndex.begin(), albumIndex.end());
    }
    myCachedMaxCount = -1;

    bool b = false;
    filterChanged(b);
}



/**
 * @warning May be called no sooner than after the repository is fully loaded.
 */
void TrackRepository::setNameFilter(const string& namePattern) {
    unsetFilter();
    myIsFilterSet = true;
    vector<reference_wrapper<TrackData>> filteredTrackData;
    for (auto& trackData: myTracksData) {
        auto name = trackData->getTrack().getName();
        if (search(name.begin(), name.end(), namePattern.begin(), namePattern.end(),
            [](char c1, char c2) {return toupper(c1) == toupper(c2); }) != name.end()) {

            filteredTrackData.push_back(*trackData);
        }
    }
    myTrackDataReferences.swap(myStoredTrackDataReferences);
    myTrackDataReferences.swap(filteredTrackData);

    myCachedMaxCount = -1;

    bool b = false;
    filterChanged(b);
}



void TrackRepository::unsetFilter() {
    if (!myIsFilterSet) {
        return;
    }
    myTrackDataReferences.clear();
    myTrackDataReferences.swap(myStoredTrackDataReferences);
    myIsFilterSet = false;
    myCachedMaxCount = -1;

    bool b = false;
    filterChanged(b);
}



void TrackRepository::onReadyTracks(vector<unique_ptr<TrackData>>& tracksData) {
    uint offset = myLoadOffset;
    auto end = offset + tracksData.size();
    if (end > myTracksData.size()) {
        myTracksData.resize(end);

        // resize references container
        for (auto idx = myTrackDataReferences.size(); idx < end; idx++) {
            myTrackDataReferences.push_back(*myTracksData[idx]);
        }
    }

    for (auto& trackData: tracksData) {
        auto& track = trackData->getTrack();
        auto& artist = myArtistRepository.getById(trackData->getArtistId());
        track.setArtist(artist);
        auto& album = myAlbumRepository.getById(trackData->getAlbumId());
        track.setAlbum(album);

        updateIndicies(*trackData);

        myTrackDataReferences[offset] = *trackData;
        myTracksData[offset] = move(trackData);
        offset++;
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, tracksData.size()};
    myLoadOffset = -1;
    myCachedMaxCount = -1;

    loaded(offsetAndLimit);

    myLoadProgress += tracksData.size();
    if (myLoadProgress >= myAmpacheService.numberOfTracks()) {
        myCache.saveTracksData(myTracksData);

        bool b = false;
        fullyLoaded(b);
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

        myTrackDataReferences.push_back(*trackData);
    }

    myLoadOffset = -1;
    myCachedMaxCount = -1;

    auto offsetAndLimit = pair<int, int>{0, myTracksData.size()};
    loaded(offsetAndLimit);

    myLoadProgress += myTracksData.size();
    bool b = false;
    fullyLoaded(b);
}



void TrackRepository::updateIndicies(TrackData& trackData) {
    auto& artist = myArtistRepository.getById(trackData.getArtistId());
    auto& albumData = myAlbumRepository.getAlbumDataById(trackData.getAlbumId());
    myIndices.updateArtistAlbum(artist, albumData);
    if (!any_of(myArtistTrackIndex[artist].begin(), myArtistTrackIndex[artist].end(),
        [&trackData](TrackData& td) {return (&td != nullptr) && (td == trackData);})) {

        myArtistTrackIndex[artist].push_back(trackData);
    }
    auto& album = albumData.getAlbum();
    if (!any_of(myAlbumTrackIndex[album].begin(), myAlbumTrackIndex[album].end(),
        [&trackData](TrackData& td) {return (&td != nullptr) && (td == trackData);})) {

        myAlbumTrackIndex[album].push_back(trackData);
    }
}



int TrackRepository::computeMaxCount() const {
    if (myIsFilterSet && myLoadProgress != 0) {
        return myTrackDataReferences.size();
    }
    return myAmpacheService.numberOfTracks();
}

}
