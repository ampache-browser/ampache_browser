// artist_filter_for_tracks.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <vector>
#include <set>
#include <memory>
#include <algorithm>
#include <functional>

#include "infrastructure/event/delegate.h"
#include "domain/artist.h"
#include "../data_objects/track_data.h"
#include "data/indices.h"
#include "data/filters/filter.h"
#include "data/filters/artist_filter_for_tracks.h"

using namespace std;
using namespace infrastructure;
using namespace domain;



namespace data {

ArtistFilterForTracks::ArtistFilterForTracks(const vector<reference_wrapper<const Artist>>& artists,
    Indices& indices): Filter<TrackData>(),
myArtists(artists),
myIndices(indices) {
    processUpdatedIndices();
    myIndices.artistTracksUpdated += DELEGATE1(&ArtistFilterForTracks::onArtistTracksUpdated,
        vector<reference_wrapper<const Artist>>);
}



ArtistFilterForTracks::~ArtistFilterForTracks() {
    myIndices.artistTracksUpdated -= DELEGATE1(&ArtistFilterForTracks::onArtistTracksUpdated,
        vector<reference_wrapper<const Artist>>);
}



void ArtistFilterForTracks::setSourceData(const vector<unique_ptr<TrackData>>&) {
    // do not call base since this filter does not use source data, it uses indices instead
}



void ArtistFilterForTracks::processUpdatedSourceData(int, int) {
    // do not call base since this filter does not use source data, it uses indices instead
}



void ArtistFilterForTracks::onArtistTracksUpdated(const vector<reference_wrapper<const Artist>>& updatedArtists) {
    auto artistsIter = find_first_of(myArtists.begin(), myArtists.end(), updatedArtists.begin(), updatedArtists.end());
    if (artistsIter != myArtists.end()) {
        processUpdatedIndices();
        changed();
    }
}



void ArtistFilterForTracks::processUpdatedIndices() {
    myFilteredData.clear();
    set<reference_wrapper<TrackData>, TrackData::NameCompare> filteredUniqueTrackData;
    for (auto& artist: myArtists) {
        auto tracksData = myIndices.getArtistTracks(artist.get());
        filteredUniqueTrackData.insert(tracksData.begin(), tracksData.end());
    }
    for (auto& trackData: filteredUniqueTrackData) {
        myFilteredData.push_back(&trackData.get());
    }
}

}
