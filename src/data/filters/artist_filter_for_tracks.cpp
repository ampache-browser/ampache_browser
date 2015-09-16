// artist_filter_for_tracks.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <vector>
#include <memory>
#include "infrastructure/event/delegate.h"
#include "domain/artist.h"
#include "../data_objects/track_data.h"
#include "data/indices.h"
#include "data/filters/artist_filter_for_tracks.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

ArtistFilterForTracks::ArtistFilterForTracks(vector<reference_wrapper<const Artist>> artists,
    Indices& indices): Filter<TrackData>(),
myArtists(artists),
myIndices(indices) {
    myIndices.changed += DELEGATE0(&ArtistFilterForTracks::onIndexChanged);
}



ArtistFilterForTracks::~ArtistFilterForTracks() {
    myIndices.changed -= DELEGATE0(&ArtistFilterForTracks::onIndexChanged);
}



void ArtistFilterForTracks::apply() {
    myFilteredData.clear();
    unordered_set<reference_wrapper<TrackData>, hash<TrackData>> filteredUniqueTrackData;

    for (auto artist: myArtists) {
        auto artistIndex = myIndices.getArtistTrack()[artist];
        filteredUniqueTrackData.insert(artistIndex.begin(), artistIndex.end());
    }
    for (auto trackData: filteredUniqueTrackData) {
        myFilteredData.push_back(trackData);
    }

    Filter<TrackData>::apply();
}



void ArtistFilterForTracks::onIndexChanged() {
    apply();
}

}
