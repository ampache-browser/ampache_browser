// track_repository.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include "data/ampache_service.h"
#include "track_data.h"
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



Track& TrackRepository::get(int offset) const {
    return myTracksData[offset]->getTrack();
}



unique_ptr<unordered_map<reference_wrapper<const Artist>, vector<reference_wrapper<AlbumData>>, hash<Artist>>>
TrackRepository::getArtistIndex() {
    return move(myArtistIndex);
}



bool TrackRepository::isLoaded(int offset, int limit) const {
    uint end = offset + limit;
    return (myTracksData.size() >= end) && all_of(myTracksData.begin() + offset, myTracksData.begin() + end,
        [](const unique_ptr<TrackData>& ad) {return ad != nullptr;});
}



int TrackRepository::maxCount() const {
    return myAmpacheService.numberOfTracks();
}



void TrackRepository::onReadyTracks(vector<unique_ptr<TrackData>>& tracksData) {
    uint offset = myLoadOffset;
    auto end = offset + tracksData.size();
    if (end > myTracksData.size()) {
        myTracksData.resize(end);
    }

    for (auto& trackData: tracksData) {
        auto& artist = myArtistRepository.getById(trackData->getArtistId());
        auto& albumData = myAlbumRepository.getAlbumDataById(trackData->getAlbumId());
        (*myArtistIndex)[artist].push_back(albumData);

        myTracksData[offset++] = move(trackData);
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, tracksData.size()};
    myLoadOffset = -1;
    loaded(offsetAndLimit);

    myLoadProgress += tracksData.size();
    if (myLoadProgress >= myAmpacheService.numberOfTracks()) {
        bool b = false;
        fullyLoaded(b);
    }
}

}
