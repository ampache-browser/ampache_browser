// selected_items.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <utility>
#include <string>
#include <vector>

#include "ui/selected_items.h"



namespace ui {

SelectedItems::SelectedItems(const std::vector<std::string>& artists, const std::vector<std::string>& albums,
                             const std::vector<std::string>& tracks):
myArtists{artists},
myAlbums{albums},
myTracks{tracks} {
}



SelectedItems::SelectedItems() {
}



SelectedItems::SelectedItems(SelectedItems&& other):
myArtists{std::move(other.myArtists)},
myAlbums{std::move(other.myAlbums)},
myTracks{std::move(other.myTracks)} {
}



SelectedItems& SelectedItems::operator=(SelectedItems&& other) {
    myArtists = std::move(other.myArtists);
    myAlbums = std::move(other.myAlbums);
    myTracks = std::move(other.myTracks);
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
