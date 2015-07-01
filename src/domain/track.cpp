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

Track::Track(const std::string id, const std::string title, int number, const std::string url):
myId{id},
myTitle{title},
myNumber{number},
myUrl{url} { }



const string Track::getId() const {
    return myId;
}



const string Track::getName() const {
    return myTitle;
}



int Track::getNumber() const {
    return myNumber;
}



const string Track::getUrl() const {
    return myUrl;
}



const Artist& Track::getArtist() const {
    return *myArtist;
}

}
