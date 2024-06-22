// track.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <cstddef>
#include <string>

#include "domain/track.h"

using namespace std;



namespace domain {

class Album;
class Artist;



Track::Track(const std::string& id, const std::string& name, const std::string& disk,
             int number, const std::string& url):
myId{id},
myName{name},
myDisk{disk},
myNumber{number},
myUrl{url} { }



const string Track::getId() const {
    return myId;
}



const string Track::getName() const {
    return myName;
}



const string Track::getDisk() const {
    return myDisk;
}



int Track::getNumber() const {
    return myNumber;
}



const string Track::getUrl() const {
    return myUrl;
}



const Artist* Track::getArtist() const {
    return myArtist;
}



const Album* Track::getAlbum() const {
    return myAlbum;
}



void Track::setArtist(const Artist& artist) {
    myArtist = &artist;
}



void Track::setAlbum(const Album& album) {
    myAlbum = &album;
}



bool operator==(const Track& lhs, const Track& rhs) {
    return lhs.getId() == rhs.getId();
}



bool operator!=(const Track& lhs, const Track& rhs) {
    return !operator==(lhs, rhs);
}



bool operator<(const Track& lhs, const Track& rhs) {
    if (lhs.getId() == rhs.getId()) {
        return false;
    }

    if (lhs.getAlbum() < rhs.getAlbum()) {
        return true;
    }
    if (lhs.getAlbum() > rhs.getAlbum()) {
        return false;
    }

    if (lhs.getDisk() < rhs.getDisk()) {
        return true;
    }
    if (lhs.getDisk() > rhs.getDisk()) {
        return false;
    }

    if (lhs.getNumber() < rhs.getNumber()) {
        return true;
    }
    if (lhs.getNumber() > rhs.getNumber()) {
        return false;
    }

    if (lhs.getName() < rhs.getName()) {
        return true;
    }
    if (lhs.getName() > rhs.getName()) {
        return false;
    }

    return lhs.getId() < rhs.getId();
}



bool operator>(const Track& lhs, const Track& rhs) {
    return operator<(rhs, lhs);
}



bool operator<=(const Track& lhs, const Track& rhs) {
    return !operator>(lhs, rhs);
}



bool operator>=(const Track& lhs, const Track& rhs) {
    return !operator<(lhs, rhs);
}

}



namespace std {

size_t hash<domain::Track>::operator()(const domain::Track& track) const {
    return hash<string>()(track.getId());
}

}
