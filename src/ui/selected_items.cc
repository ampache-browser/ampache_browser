// selected_items.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <string>
#include <vector>
#include "ui/selected_items.h"

using namespace std;



namespace ui {

SelectedItems::SelectedItems(const vector<string>& artists, const vector<string>& albums, const vector<string>& tracks):
myArtists{artists},
myAlbums{albums},
myTracks{tracks} {
}



SelectedItems::SelectedItems() {
}



SelectedItems::SelectedItems(SelectedItems&& other):
myArtists{move(other.myArtists)},
myAlbums{move(other.myAlbums)},
myTracks{move(other.myTracks)} {
}



SelectedItems& SelectedItems::operator=(SelectedItems&& other) {
    myArtists = move(other.myArtists);
    myAlbums = move(other.myAlbums);
    myTracks = move(other.myTracks);
    return *this;
}



std::vector<std::string> SelectedItems::getArtists() const {
    return myArtists;
}



std::vector<std::string> SelectedItems::getAlbums() const {
    return myAlbums;
}



std::vector<std::string> SelectedItems::getTracks() const {
    return myTracks;
}

}
