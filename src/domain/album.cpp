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



bool operator==(const Album& lhs, const Album& rhs) {
    return lhs.getId() == rhs.getId();
}



bool operator!=(const Album& lhs, const Album& rhs) {
    return !operator==(lhs, rhs);
}



bool operator<(const Album& lhs, const Album& rhs) {
    return (lhs.getId() != rhs.getId()) && (lhs.getName() < rhs.getName());
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
