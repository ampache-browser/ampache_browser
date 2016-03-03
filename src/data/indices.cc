// indices.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include "domain/artist.h"
#include "data/indices.h"

using namespace std;
using namespace domain;



namespace data {

void Indices::addArtist(const Artist& artist) {
    myArtistAlbums[artist] = move(AlbumDataUnorderedSet{});
    myArtistTracks[artist] = move(TrackDataUnorderedSet{});
    changed();
}



void Indices::addAlbum(const Album& album) {
    myAlbumTracks[album] = move(TrackDataUnorderedSet{});
    changed();
}



AlbumDataUnorderedSet& Indices::getArtistAlbums(const Artist& artist) {
    return myArtistAlbums.at(artist);
}



void Indices::updateArtistAlbums(const Artist& artist, AlbumData& albumData) {
    myArtistAlbums[artist].insert(albumData);
    changed();
}



TrackDataUnorderedSet& Indices::getArtistTracks(const Artist& artist) {
    return myArtistTracks.at(artist);
}



void Indices::updateArtistTracks(const Artist& artist, TrackData& trackData) {
    myArtistTracks[artist].insert(trackData);
    changed();
}



TrackDataUnorderedSet& Indices::getAlbumTracks(const Album& album) {
    return myAlbumTracks.at(album);
}



void Indices::updateAlbumTracks(const Album& album, TrackData& trackData) {
    myAlbumTracks[album].insert(trackData);
    changed();
}



void Indices::clearArtists() {
    myArtistAlbums.clear();
    myArtistTracks.clear();
    changed();
}



void Indices::clearAlbums() {
    myAlbumTracks.clear();
    changed();
}



void Indices::clearArtistsAlbums() {
    for (auto& artistAndAlbums: myArtistAlbums) {
        artistAndAlbums.second.clear();
    }
}



void Indices::clearArtistsTracks() {
    for (auto& artistAndTracls: myArtistTracks) {
        artistAndTracls.second.clear();
    }
}



void Indices::clearAlbumsTracks() {
    for (auto& albumAndTracks: myAlbumTracks) {
        albumAndTracks.second.clear();
    }
}

}
