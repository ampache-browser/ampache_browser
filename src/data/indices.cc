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

AlbumDataUnorderedSet& Indices::getArtistAlbums(const Artist& artist) {
    return myArtistAlbum.at(artist);
}



void Indices::updateArtistAlbum(const Artist& artist, AlbumData& albumData) {
    myArtistAlbum[artist].insert(albumData);
    changed();
}



TrackDataUnorderedSet& Indices::getArtistTracks(const Artist& artist) {
    return myArtistTrack.at(artist);
}



void Indices::updateArtistTrack(const Artist& artist, TrackData& trackData) {
    myArtistTrack[artist].insert(trackData);
    changed();
}



TrackDataUnorderedSet& Indices::getAlbumTracks(const Album& album) {
    return myAlbumTrack.at(album);
}



void Indices::updateAlbumTrack(const Album& album, TrackData& trackData) {
    myAlbumTrack[album].insert(trackData);
    changed();
}

}
