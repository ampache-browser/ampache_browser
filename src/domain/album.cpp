// album.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <string>
#include <deque>
#include <QtGui/QPixmap>
#include "domain/artist.h"
#include "domain/track.h"
#include "domain/album.h"

using namespace std;



namespace domain {



Album::Album(const string& id, const string& name, int releaseYear):
myId{id},
myName{name},
myReleaseYear{releaseYear} {
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



const Artist& Album::getArtist() const {
    return *myArtist;
}



void Album::setArtist(const Artist& artist) {
    myArtist = &artist;
}



bool Album::hasArt() const {
    return myArt != nullptr;
}



QPixmap& Album::getArt() const {
    return *myArt;
}



void Album::setArt(unique_ptr<QPixmap> art) {
    myArt = move(art);
}



void Album::addTrack(const Track& track) {
    myTracks.push_back(&track);
}

}
