// artist.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ARTIST_H
#define ARTIST_H



#include <string>



namespace domain {

class Artist {

public:
    Artist(const std::string &name);

    ~Artist();

    const std::string getName() const;

private:
    const std::string myName;
};

}


#endif // ARTIST_H
