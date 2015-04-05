// track.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef TRACK_H
#define TRACK_H



class Artist;

#include <string>



namespace domain {

class Track {

public:
    Track(const std::string &name, Artist* artist);

    ~Track();

    const std::string getName() const;

    Artist* getArtist() const;

private:
    const std::string myName;
    Artist* myArtist;
};

}


#endif // TRACK_H
