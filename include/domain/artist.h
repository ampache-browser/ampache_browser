// artist.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ARTIST_H
#define ARTIST_H



#include <string>



namespace domain {

class Artist {

public:
    Artist(const std::string& id, const std::string& name);

    Artist(const Artist& other) = delete;

    Artist& operator=(const Artist& other) = delete;

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



namespace std {

template<>
class hash<domain::Artist> {

public:
    size_t operator()(const domain::Artist& artist) const;
};

}



#endif // ARTIST_H
