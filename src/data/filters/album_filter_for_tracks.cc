// album_filter_for_tracks.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <vector>
#include <set>
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
    set<reference_wrapper<TrackData>> filteredUniqueTrackData;
    for (auto album: myAlbums) {
        auto albumIndex = myIndices.getAlbumTrack()[album];
        filteredUniqueTrackData.insert(albumIndex.begin(), albumIndex.end());
    }
    myFilteredData = vector<reference_wrapper<TrackData>>{
        filteredUniqueTrackData.begin(), filteredUniqueTrackData.end()};

    Filter<TrackData>::apply();
}



void AlbumFilterForTracks::onIndexChanged() {
    apply();
}

}
