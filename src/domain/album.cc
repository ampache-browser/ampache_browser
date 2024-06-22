// album.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <cstddef>
#include <memory>
#include <string>
#include <utility>

#include <QtGui/QPixmap>

#include "domain/album.h"

using namespace std;



namespace domain {

class Artist;



Album::Album(const string& id, const string& name, int releaseYear, int mediaNumber):
myId{id},
myName{name},
myReleaseYear{releaseYear},
myMediaNumber{mediaNumber} {
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



int Album::getMediaNumber() const {
    return myMediaNumber;
}



const Artist* Album::getArtist() const {
    return myArtist;
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



bool operator==(const Album& lhs, const Album& rhs) {
    return lhs.getId() == rhs.getId();
}



bool operator!=(const Album& lhs, const Album& rhs) {
    return !operator==(lhs, rhs);
}



bool operator<(const Album& lhs, const Album& rhs) {
    if (lhs.getId() == rhs.getId()) {
        return false;
    }

    if (lhs.getName() < rhs.getName()) {
        return true;
    }
    if (lhs.getName() > rhs.getName()) {
        return false;
    }

    if (lhs.getMediaNumber() < rhs.getMediaNumber()) {
        return true;
    }
    if (lhs.getMediaNumber() > rhs.getMediaNumber()) {
        return false;
    }

    return lhs.getId() < rhs.getId();
}



bool operator>(const Album& lhs, const Album& rhs) {
    return operator<(rhs, lhs);
}



bool operator<=(const Album& lhs, const Album& rhs) {
    return !operator>(lhs, rhs);
}



bool operator>=(const Album& lhs, const Album& rhs) {
    return !operator<(lhs, rhs);
}

}



namespace std {

size_t hash<domain::Album>::operator()(const domain::Album& album) const {
    return hash<string>()(album.getId());
}

}
