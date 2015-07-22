// album_data.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <string>
#include <memory>
#include "domain/album.h"
#include "album_data.h"

using namespace std;
using namespace domain;



namespace data {

AlbumData::AlbumData(const string& id, const string& artUrl, const string& artistId, unique_ptr<Album> album):
myId{id},
myArtUrl{artUrl},
myArtistId{artistId},
myAlbum{move(album)} {
}



string AlbumData::getId() const {
    return myId;
}



string AlbumData::getArtUrl() const {
    return myArtUrl;
}



string AlbumData::getArtistId() const {
    return myArtistId;
}



Album& AlbumData::getAlbum() const {
    return *myAlbum;
}



bool AlbumData::hasArtist() const {
    return myArtistId != "0";
}

}
