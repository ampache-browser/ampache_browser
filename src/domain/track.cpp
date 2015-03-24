// track.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <string>
#include <domain/artist.h>
#include <domain/track.h>

using namespace std;



namespace domain {

Track::Track(const string &name, Artist* artist):
myName{name},
myArtist{artist} {
}



Track::~Track() {
}



const string Track::getName() const {
    return myName;
}



Artist* Track::getArtist() const {
    return myArtist;
}

}
