// album_artist_filter.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <vector>
#include <memory>
#include "infrastructure/event.h"
#include "domain/artist.h"
#include "../data_objects/album_data.h"
#include "data/indices.h"
#include "album_artist_filter.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

AlbumArtistFilter::AlbumArtistFilter(vector<unique_ptr<AlbumData>>& sourceData,
    vector<reference_wrapper<const Artist>> artists, Indices& indices): Filter<AlbumData>(sourceData),
myArtists(artists),
myIndices(indices) {
    myIndices.changed += bind(&AlbumArtistFilter::onIndexChanged, this, _1);
}



AlbumArtistFilter::~AlbumArtistFilter() {
    myIndices.changed -= bind(&AlbumArtistFilter::onIndexChanged, this, _1);
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



void AlbumArtistFilter::onIndexChanged(bool&) {
    apply();
}

}
