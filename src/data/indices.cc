// indices.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <functional>
#include <utility>
#include <vector>

#include "domain/artist.h"
#include "domain/album.h"
#include "data/indices.h"

using namespace std;
using namespace domain;



namespace data {

void Indices::addArtists(const vector<reference_wrapper<Artist>>& artists) {
    for (auto& artist: artists) {
        myArtistAlbums[artist.get()] = AlbumDataUnorderedSet{};
        myArtistTracks[artist.get()] = TrackDataUnorderedSet{};
    }
}



void Indices::addAlbums(const vector<reference_wrapper<Album>>& albums) {
    for (auto& album: albums) {
        myAlbumTracks[album.get()] = TrackDataUnorderedSet{};
    }
}



AlbumDataUnorderedSet& Indices::getArtistAlbums(const Artist& artist) {
    return myArtistAlbums.at(artist);
}



void Indices::updateArtistAlbums(const ArtistAlbumsIndex& artistAlbums) {
    vector<reference_wrapper<const Artist>> updatedArtists;
    for (auto& artistAndAlbums: artistAlbums) {
        myArtistAlbums[artistAndAlbums.first].insert(artistAndAlbums.second.begin(), artistAndAlbums.second.end());
        updatedArtists.push_back(artistAndAlbums.first);
    }
    artistAlbumsUpdated(updatedArtists);
}



TrackDataUnorderedSet& Indices::getArtistTracks(const Artist& artist) {
    return myArtistTracks.at(artist);
}



void Indices::updateArtistTracks(const ArtistTracksIndex& artistTracks) {
    vector<reference_wrapper<const Artist>> updatedArtists;
    for (auto& artistAndTracks: artistTracks) {
        myArtistTracks[artistAndTracks.first].insert(artistAndTracks.second.begin(), artistAndTracks.second.end());
        updatedArtists.push_back(artistAndTracks.first);
    }
    artistTracksUpdated(updatedArtists);
}



TrackDataUnorderedSet& Indices::getAlbumTracks(const Album& album) {
    return myAlbumTracks.at(album);
}



void Indices::updateAlbumTracks(const AlbumTracksIndex& albumTracks) {
    vector<reference_wrapper<const Album>> updatedAlbums;
    for (auto& albumAndTracks: albumTracks) {
        myAlbumTracks[albumAndTracks.first].insert(albumAndTracks.second.begin(), albumAndTracks.second.end());
        updatedAlbums.push_back(albumAndTracks.first);
    }
    albumTracksUpdated(updatedAlbums);
}



void Indices::clearArtists() {
    myArtistAlbums.clear();
    myArtistTracks.clear();
}



void Indices::clearAlbums() {
    myAlbumTracks.clear();
}



void Indices::clearArtistsAlbums() {
    for (auto& artistAndAlbums: myArtistAlbums) {
        artistAndAlbums.second.clear();
    }
    vector<reference_wrapper<const Artist>> updatedArtists;
    artistAlbumsUpdated(updatedArtists);
}



void Indices::clearArtistsTracks() {
    for (auto& artistAndTracls: myArtistTracks) {
        artistAndTracls.second.clear();
    }
    vector<reference_wrapper<const Artist>> updatedArtists;
    artistTracksUpdated(updatedArtists);
}



void Indices::clearAlbumsTracks() {
    for (auto& albumAndTracks: myAlbumTracks) {
        albumAndTracks.second.clear();
    }
    vector<reference_wrapper<const Album>> updatedAlbums;
    albumTracksUpdated(updatedAlbums);
}

}
