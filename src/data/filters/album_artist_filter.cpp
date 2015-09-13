// album_artist_filter.cpp
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
#include "data/filters/album_artist_filter.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

AlbumArtistFilter::AlbumArtistFilter(vector<reference_wrapper<const Artist>> artists,
    Indices& indices): Filter<AlbumData>(),
myArtists(artists),
myIndices(indices) {
    myIndices.changed += DELEGATE0(&AlbumArtistFilter::onIndexChanged);
}



AlbumArtistFilter::~AlbumArtistFilter() {
    myIndices.changed -= DELEGATE0(&AlbumArtistFilter::onIndexChanged);
}



void AlbumArtistFilter::apply() {
    myFilteredData.clear();
    unordered_set<reference_wrapper<AlbumData>, hash<AlbumData>> filteredUniqueAlbumData;

    // TODO: Include also albums that have matching album artist.
    for (auto artist: myArtists) {
        auto artistIndex = myIndices.getArtistAlbum()[artist];
        filteredUniqueAlbumData.insert(artistIndex.begin(), artistIndex.end());
    }
    for (auto albumData: filteredUniqueAlbumData) {
        myFilteredData.push_back(albumData);
    }

    Filter<AlbumData>::apply();
}



void AlbumArtistFilter::onIndexChanged() {
    apply();
}

}
