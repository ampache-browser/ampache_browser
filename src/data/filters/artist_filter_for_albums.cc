// artist_filter_for_albums.cc
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
#include "data/filters/artist_filter_for_albums.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

ArtistFilterForAlbums::ArtistFilterForAlbums(vector<reference_wrapper<const Artist>> artists,
    Indices& indices): Filter<AlbumData>(),
myArtists(artists),
myIndices(indices) {
    myIndices.changed += DELEGATE0(&ArtistFilterForAlbums::onIndexChanged);
}



ArtistFilterForAlbums::~ArtistFilterForAlbums() {
    myIndices.changed -= DELEGATE0(&ArtistFilterForAlbums::onIndexChanged);
}



void ArtistFilterForAlbums::apply() {
    myFilteredData.clear();
    set<reference_wrapper<AlbumData>> filteredUniqueAlbumData;

    // TODO: Include also albums that have matching album artist.
    for (auto artist: myArtists) {
        auto artistIndex = myIndices.getArtistAlbum()[artist];
        filteredUniqueAlbumData.insert(artistIndex.begin(), artistIndex.end());
    }
    myFilteredData =
        vector<reference_wrapper<AlbumData>>{filteredUniqueAlbumData.begin(), filteredUniqueAlbumData.end()};

    Filter<AlbumData>::apply();
}



void ArtistFilterForAlbums::onIndexChanged() {
    apply();
}

}
