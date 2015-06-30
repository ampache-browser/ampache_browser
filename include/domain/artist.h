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
    Artist(const std::string id, const std::string name);

    const std::string getId() const;

    const std::string getName() const;

private:
    const std::string myId;
    const std::string myName;
};

bool operator==(const Artist& lhs, const Artist& rhs);

bool operator!=(const Artist& lhs, const Artist& rhs);

bool operator<(const Artist& lhs, const Artist& rhs);

bool operator>(const Artist& lhs, const Artist& rhs);

bool operator<=(const Artist& lhs, const Artist& rhs);

bool operator>=(const Artist& lhs, const Artist& rhs);

}


#endif // ARTIST_H
