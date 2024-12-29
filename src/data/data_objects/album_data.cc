// album_data.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <utility>
#include <string>
#include <memory>

#include "domain/album.h"
#include "album_data.h"

using namespace domain;



namespace data {

AlbumData::AlbumData(const std::string& id, const std::string& artUrl, const std::string& artistId, int numberOfTracks,
    std::unique_ptr<Album> album):
myId{id},
myArtUrl{artUrl},
myArtistId{artistId},
myNumberOfTracks{numberOfTracks},
myAlbum{std::move(album)} {
}



std::string AlbumData::getId() const {
    return myId;
}



std::string AlbumData::getArtUrl() const {
    return myArtUrl;
}



std::string AlbumData::getArtistId() const {
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



size_t std::hash<data::AlbumData>::operator()(const data::AlbumData& albumData) const {
    return std::hash<std::string>()(albumData.getId());
}
