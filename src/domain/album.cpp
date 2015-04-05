// album.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <string>
#include <vector>
#include <QtGui/QPixmap>
#include "domain/artist.h"
#include "domain/track.h"
#include "domain/album.h"

using namespace std;



namespace domain {

Album::Album(const string id, const string name, int releaseYear):
myId{id},
myName{name},
myReleaseYear{releaseYear} {
}



Album::~Album() {
}



const string Album::getId() const {
    return myId;
}



const string Album::getName() const {
    return myName;
}



int Album::getReleaseYear() const {
    return myReleaseYear;
}



Artist* Album::getArtist() const {
    return myArtist;
}



void Album::setArtist(Artist* artist) {
    myArtist = artist;
}



QPixmap* Album::getArt() const {
    return myArt;
}



void Album::setArt(QPixmap* art) {
    myArt = art;
}



void Album::addTrack(Track* track) {
    myTracks.push_back(track);
}

}
