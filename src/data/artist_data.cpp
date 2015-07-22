// artist_data.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <string>
#include <memory>
#include "domain/artist.h"
#include "artist_data.h"

using namespace std;
using namespace domain;



namespace data {

ArtistData::ArtistData(const string& id, int numberOfAlbums, unique_ptr<Artist> artist):
myId{id},
myNumberOfAlbums{numberOfAlbums},
myArtist{move(artist)} {
}



string ArtistData::getId() const {
    return myId;
}



int ArtistData::getNumberOfAlbums() const {
    return myNumberOfAlbums;
}



Artist& ArtistData::getArtist() const {
    return *myArtist;
}

}
