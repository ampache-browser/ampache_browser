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

void Indices::addArtists(const vector<reference_wrapper<Artist>>& artists) {
    for (auto& artist: artists) {
        myArtistAlbums[artist.get()] = move(AlbumDataUnorderedSet{});
        myArtistTracks[artist.get()] = move(TrackDataUnorderedSet{});
    }
    changed();
}



void Indices::addAlbums(const vector<reference_wrapper<Album>>& albums) {
    for (auto& album: albums) {
        myAlbumTracks[album.get()] = move(TrackDataUnorderedSet{});
    }
    changed();
}



AlbumDataUnorderedSet& Indices::getArtistAlbums(const Artist& artist) {
    return myArtistAlbums.at(artist);
}



void Indices::updateArtistAlbums(const ArtistAlbumsIndex& artistAlbums) {
    for (auto& artistAndAlbums: artistAlbums) {
        myArtistAlbums[artistAndAlbums.first].insert(artistAndAlbums.second.begin(), artistAndAlbums.second.end());
    }
    changed();
}



TrackDataUnorderedSet& Indices::getArtistTracks(const Artist& artist) {
    return myArtistTracks.at(artist);
}



void Indices::updateArtistTracks(const ArtistTracksIndex& artistTracks) {
    for (auto& artistAndTracks: artistTracks) {
        myArtistTracks[artistAndTracks.first].insert(artistAndTracks.second.begin(), artistAndTracks.second.end());
    }
    changed();
}



TrackDataUnorderedSet& Indices::getAlbumTracks(const Album& album) {
    return myAlbumTracks.at(album);
}



void Indices::updateAlbumTracks(const AlbumTracksIndex& albumTracks) {
    for (auto& albumAndTracks: albumTracks) {
        myAlbumTracks[albumAndTracks.first].insert(albumAndTracks.second.begin(), albumAndTracks.second.end());
    }
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
