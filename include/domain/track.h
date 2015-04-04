// track.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef TRACK_H
#define TRACK_H



#include <string>
#include "domain/artist.h"

using namespace std;



namespace domain {

class Track {

public:
    Track(const string &name, Artist* artist);

    ~Track();

    const string getName() const;

    Artist* getArtist() const;

private:
    const string myName;
    Artist* myArtist;
};

}


#endif // TRACK_H
