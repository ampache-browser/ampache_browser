// track.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef TRACK_H
#define TRACK_H



#include <string>



namespace domain {

class Artist;



class Track {

public:
    Track(const std::string id, const std::string title, int number, const std::string url);

    const std::string getId() const;

    const std::string getName() const;

    int getNumber() const;

    const std::string getUrl() const;

    // TODO: return reference
    Artist* getArtist() const;

private:
    const std::string myId;
    const std::string myTitle;
    const int myNumber;
    const std::string myUrl;

    Artist* myArtist;
};

}


#endif // TRACK_H
