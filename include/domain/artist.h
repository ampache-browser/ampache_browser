// artist.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ARTIST_H
#define ARTIST_H



#include <string>

using namespace std;



namespace domain {

class Artist {

public:
    Artist(const string &name);

    ~Artist();

    const string getName() const;

private:
    const string myName;
};

}


#endif // ARTIST_H
