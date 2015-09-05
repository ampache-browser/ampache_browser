// indices.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include "domain/artist.h"
#include "data/indices.h"

using namespace std;
using namespace domain;



namespace data {

ArtistAlbumIndex& Indices::getArtistAlbum() {
    return myArtistAlbum;
}



void Indices::updateArtistAlbum(Artist& artist, AlbumData& albumData) {
    myArtistAlbum[artist].insert(albumData);
    bool b = false;
    changed(b);
}

}
