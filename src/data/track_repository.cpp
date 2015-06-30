// track_repository.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include "data/ampache_service.h"
#include "track_data.h"
#include "data/track_repository.h"

using namespace std;
using namespace placeholders;
using namespace domain;



namespace data {

TrackRepository::TrackRepository(AmpacheService& ampacheService):
myAmpacheService(ampacheService) {
    myAmpacheService.readyTracks += bind(&TrackRepository::onReadyTracks, this, _1);
}



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
        myTracksData[offset++] = move(trackData);
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, tracksData.size()};
    myLoadOffset = -1;
    loaded(offsetAndLimit);
}

}
