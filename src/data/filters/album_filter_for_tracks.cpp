// album_filter_for_tracks.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <vector>
#include <memory>
#include "infrastructure/event/delegate.h"
#include "domain/artist.h"
#include "../data_objects/album_data.h"
#include "data/indices.h"
#include "data/filters/album_filter_for_tracks.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

AlbumFilterForTracks::AlbumFilterForTracks(vector<reference_wrapper<const Album>> albums,
    Indices& indices): Filter<TrackData>(),
myAlbums(albums),
myIndices(indices) {
    myIndices.changed += DELEGATE0(&AlbumFilterForTracks::onIndexChanged);
}



AlbumFilterForTracks::~AlbumFilterForTracks() {
    myIndices.changed -= DELEGATE0(&AlbumFilterForTracks::onIndexChanged);
}



void AlbumFilterForTracks::apply() {
    myFilteredData.clear();
    unordered_set<reference_wrapper<TrackData>, hash<TrackData>> filteredUniqueTrackData;

    for (auto album: myAlbums) {
        auto albumIndex = myIndices.getAlbumTrack()[album];
        filteredUniqueTrackData.insert(albumIndex.begin(), albumIndex.end());
    }
    for (auto trackData: filteredUniqueTrackData) {
        myFilteredData.push_back(trackData);
    }

    Filter<TrackData>::apply();
}



void AlbumFilterForTracks::onIndexChanged() {
    apply();
}

}
