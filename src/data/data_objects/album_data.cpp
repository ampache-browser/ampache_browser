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

AlbumData::AlbumData(const string& id, const string& artUrl, const string& artistId, int numberOfTracks,
    unique_ptr<Album> album):
myId{id},
myArtUrl{artUrl},
myArtistId{artistId},
myNumberOfTracks{numberOfTracks},
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



int AlbumData::getNumberOfTracks() const {
    return myNumberOfTracks;
}



Album& AlbumData::getAlbum() const {
    return *myAlbum;
}



bool AlbumData::hasArtist() const {
    return myArtistId != "0";
}



bool operator==(const AlbumData& lhs, const AlbumData& rhs) {
    return lhs.getId() == rhs.getId();
}



bool operator!=(const AlbumData& lhs, const AlbumData& rhs) {
    return !operator==(lhs, rhs);
}



bool operator<(const AlbumData& lhs, const AlbumData& rhs) {
    return (lhs.getId() != rhs.getId()) && (lhs.getAlbum() < rhs.getAlbum());
}



bool operator>(const AlbumData& lhs, const AlbumData& rhs) {
    return operator<(rhs, lhs);
}



bool operator<=(const AlbumData& lhs, const AlbumData& rhs) {
    return !operator>(lhs, rhs);
}



bool operator>=(const AlbumData& lhs, const AlbumData& rhs) {
    return !operator<(lhs, rhs);
}

}



namespace std {

size_t hash<data::AlbumData>::operator()(const data::AlbumData& albumData) const {
    return hash<string>()(albumData.getId());
}

}
