// track_repository.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include "data/ampache_service.h"
#include "track_data.h"
#include "album_data.h"
#include "index_types.h"
#include "data/artist_repository.h"
#include "data/album_repository.h"
#include "data/track_repository.h"

using namespace std;
using namespace placeholders;
using namespace domain;



namespace data {

TrackRepository::TrackRepository(AmpacheService& ampacheService, ArtistRepository& artistRepository,
    AlbumRepository& albumRepository):
myAmpacheService(ampacheService),
myArtistRepository(artistRepository),
myAlbumRepository(albumRepository) {
    myAmpacheService.readyTracks += bind(&TrackRepository::onReadyTracks, this, _1);
}



/**
 * @warning Class does not work correctly if this method is called multiple times for the same data.
 */
bool TrackRepository::load(int offset, int limit) {
    if (myLoadOffset != -1) {
        return false;
    }
    myLoadOffset = offset;
    myAmpacheService.requestTracks(offset, limit);
    return true;
}



Track& TrackRepository::get(int filteredOffset) const {
    TrackData& trackData = myTrackDataReferences[filteredOffset];
    return trackData.getTrack();
}



unique_ptr<ArtistAlbumVectorIndex> TrackRepository::getArtistAlbumIndex() {
    unique_ptr<ArtistAlbumVectorIndex> vectorArtistIndex{new ArtistAlbumVectorIndex};

    for (auto& artistAndAlbumData: myArtistAlbumIndex) {
        vector<reference_wrapper<AlbumData>> albumsData;
        for (auto& albumData: artistAndAlbumData.second) {
            albumsData.push_back(albumData);
        }
        (*vectorArtistIndex)[artistAndAlbumData.first] = albumsData;
    }

    return move(vectorArtistIndex);
}



bool TrackRepository::isLoaded(int filteredOffset, int limit) const {
    uint end = filteredOffset + limit;

    return (myTrackDataReferences.size() >= end) && all_of(myTrackDataReferences.begin() + filteredOffset,
        myTrackDataReferences.begin() + filteredOffset + limit, [](const TrackData& td) {return &td != nullptr;});
}



int TrackRepository::maxCount() const {
    if (myCurrentArtistFilter != nullptr) {
        return myArtistRepository.getArtistDataById(myCurrentArtistFilter->getId()).getNumberOfTracks();
    }
    if (myCurrentAlbumFilter != nullptr) {
        return myAlbumRepository.getAlbumDataById(myCurrentAlbumFilter->getId()).getNumberOfTracks();
    }
    return myAmpacheService.numberOfTracks();
}



/**
 * @warning May be called no sooner than after the repository is fully loaded.
 */
void TrackRepository::setArtistFilter(const Artist& artist) {
    unsetAlbumFilter();
    unsetArtistFilter();
    myCurrentArtistFilter = &artist;
    myArtistTrackIndex[*myCurrentArtistFilter].swap(myTrackDataReferences);

    bool b = false;
    filterChanged(b);
}



void TrackRepository::unsetArtistFilter() {
    if (myCurrentArtistFilter == nullptr) {
        return;
    }
    myTrackDataReferences.swap(myArtistTrackIndex[*myCurrentArtistFilter]);
    myCurrentArtistFilter = nullptr;

    bool b = false;
    filterChanged(b);
}



/**
 * @warning May be called no sooner than after the repository is fully loaded.
 */
void TrackRepository::setAlbumFilter(const Album& album) {
    unsetArtistFilter();
    unsetAlbumFilter();
    myCurrentAlbumFilter = &album;
    myAlbumTrackIndex[*myCurrentAlbumFilter].swap(myTrackDataReferences);

    bool b = false;
    filterChanged(b);
}



void TrackRepository::unsetAlbumFilter() {
    if (myCurrentAlbumFilter == nullptr) {
        return;
    }
    myTrackDataReferences.swap(myAlbumTrackIndex[*myCurrentAlbumFilter]);
    myCurrentAlbumFilter = nullptr;

    bool b = false;
    filterChanged(b);
}



void TrackRepository::onReadyTracks(vector<unique_ptr<TrackData>>& tracksData) {
    auto storedCurrentArtistFilter = myCurrentArtistFilter;
    unsetArtistFilter();

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
    if (storedCurrentArtistFilter != nullptr) {
        setArtistFilter(*storedCurrentArtistFilter);
    }

    loaded(offsetAndLimit);

    myLoadProgress += tracksData.size();
    if (myLoadProgress >= myAmpacheService.numberOfTracks()) {
        bool b = false;
        fullyLoaded(b);
    }
}



void TrackRepository::updateIndicies(TrackData& trackData) {
    auto& artist = myArtistRepository.getById(trackData.getArtistId());
    auto& albumData = myAlbumRepository.getAlbumDataById(trackData.getAlbumId());
    myArtistAlbumIndex[artist].insert(albumData);
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

}
