// album_filter_for_tracks.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



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

AlbumFilterForTracks::AlbumFilterForTracks(const vector<reference_wrapper<const Album>>& albums,
    Indices& indices): Filter<TrackData>(),
myAlbums(albums),
myIndices(indices) {
    processUpdatedIndices();
    myIndices.albumTracksUpdated += DELEGATE1(&AlbumFilterForTracks::onAlbumTracksUpdated,
        vector<reference_wrapper<const Album>>);
}



AlbumFilterForTracks::~AlbumFilterForTracks() {
    myIndices.albumTracksUpdated -= DELEGATE1(&AlbumFilterForTracks::onAlbumTracksUpdated,
        vector<reference_wrapper<const Album>>);
}



void AlbumFilterForTracks::setSourceData(const vector<unique_ptr<TrackData>>&) {
    // do not call base since this filter does not use source data, it uses indices instead
}



void AlbumFilterForTracks::processUpdatedSourceData(int, int) {
    // do not call base since this filter does not use source data, it uses indices instead
}



void AlbumFilterForTracks::onAlbumTracksUpdated(const vector<reference_wrapper<const Album>>& updatedAlbums) {
    auto albumsIter = find_first_of(myAlbums.begin(), myAlbums.end(), updatedAlbums.begin(), updatedAlbums.end());
    if (albumsIter != myAlbums.end()) {
        processUpdatedIndices();
        changed();
    }
}



void AlbumFilterForTracks::processUpdatedIndices() {
    myFilteredData.clear();
    set<reference_wrapper<TrackData>> filteredUniqueTrackData;
    for (auto& album: myAlbums) {
        auto tracksData = myIndices.getAlbumTracks(album.get());
        filteredUniqueTrackData.insert(tracksData.begin(), tracksData.end());
    }
    for (auto& trackData: filteredUniqueTrackData) {
        myFilteredData.push_back(&trackData.get());
    }
}

}
