// artist_data.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <utility>
#include <string>
#include <memory>

#include "domain/artist.h"
#include "artist_data.h"

using namespace domain;



namespace data {

ArtistData::ArtistData(const std::string& id, int numberOfAlbums, int numberOfTracks, std::unique_ptr<Artist> artist):
myId{id},
myNumberOfAlbums{numberOfAlbums},
myNumberOfTracks{numberOfTracks},
myArtist{std::move(artist)} {
}



std::string ArtistData::getId() const {
    return myId;
}



int ArtistData::getNumberOfAlbums() const {
    return myNumberOfAlbums;
}



int ArtistData::getNumberOfTracks() const {
    return myNumberOfTracks;
}



Artist& ArtistData::getArtist() const {
    return *myArtist;
}



bool operator==(const ArtistData& lhs, const ArtistData& rhs) {
    return lhs.getId() == rhs.getId();
}



bool operator!=(const ArtistData& lhs, const ArtistData& rhs) {
    return !operator==(lhs, rhs);
}



bool operator<(const ArtistData& lhs, const ArtistData& rhs) {
    return (lhs.getId() != rhs.getId()) && (lhs.getArtist() < rhs.getArtist());
}



bool operator>(const ArtistData& lhs, const ArtistData& rhs) {
    return operator<(rhs, lhs);
}



bool operator<=(const ArtistData& lhs, const ArtistData& rhs) {
    return !operator>(lhs, rhs);
}



bool operator>=(const ArtistData& lhs, const ArtistData& rhs) {
    return !operator<(lhs, rhs);
}

}



size_t std::hash<data::ArtistData>::operator()(const data::ArtistData& artistData) const {
    return std::hash<string>()(artistData.getId());
}
