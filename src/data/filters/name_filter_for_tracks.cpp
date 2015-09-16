// name_filter_for_tracks.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include "../data_objects/track_data.h"
#include "data/filters/name_filter_for_tracks.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;



namespace data {

NameFilterForTracks::NameFilterForTracks(const string& namePattern): Filter<TrackData>(),
myNamePattern(namePattern) {
}



void NameFilterForTracks::apply() {
    myFilteredData.clear();

    for (auto& trackData: *mySourceData) {
        if (trackData == nullptr) {
            continue;
        }
        auto name = trackData->getTrack().getName();
        if (search(name.begin(), name.end(), myNamePattern.begin(), myNamePattern.end(),
            [](char c1, char c2) {return toupper(c1) == toupper(c2); }) != name.end()) {

            myFilteredData.push_back(*trackData);
        }
    }

    Filter<TrackData>::apply();
}

}
