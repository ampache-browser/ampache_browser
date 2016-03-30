// artist_filter_for_albums.cc
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
#include "data/filters/artist_filter_for_albums.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

ArtistFilterForAlbums::ArtistFilterForAlbums(const vector<reference_wrapper<const Artist>>& artists,
    Indices& indices): Filter<AlbumData>(),
myArtists(artists),
myIndices(indices) {
    processUpdatedIndices();
    myIndices.artistAlbumsUpdated += DELEGATE1(&ArtistFilterForAlbums::onArtistAlbumsUpdated,
        vector<reference_wrapper<const Artist>>);
}



ArtistFilterForAlbums::~ArtistFilterForAlbums() {
    myIndices.artistAlbumsUpdated -= DELEGATE1(&ArtistFilterForAlbums::onArtistAlbumsUpdated,
        vector<reference_wrapper<const Artist>>);
}



void ArtistFilterForAlbums::setSourceData(const vector<unique_ptr<AlbumData>>&) {
    // do not call base since this filter does not use source data, it uses indices instead
}


void ArtistFilterForAlbums::processUpdatedSourceData(int, int) {
    // do not call base since this filter does not use source data, it uses indices instead
}



void ArtistFilterForAlbums::onArtistAlbumsUpdated(const vector<reference_wrapper<const Artist>>& updatedArtists) {
    auto artistsIter = find_first_of(myArtists.begin(), myArtists.end(), updatedArtists.begin(), updatedArtists.end());
    if (artistsIter != myArtists.end()) {
        processUpdatedIndices();
        changed();
    }
}



void ArtistFilterForAlbums::processUpdatedIndices() {
    myFilteredData.clear();
    set<reference_wrapper<AlbumData>> filteredUniqueAlbumData;

    for (auto& artist: myArtists) {
        auto albumsData = myIndices.getArtistAlbums(artist.get());
        filteredUniqueAlbumData.insert(albumsData.begin(), albumsData.end());
    }
    myFilteredData =
        vector<reference_wrapper<AlbumData>>{filteredUniqueAlbumData.begin(), filteredUniqueAlbumData.end()};
}

}
