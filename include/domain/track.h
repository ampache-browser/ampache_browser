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
class Album;



class Track {

public:
    Track(const std::string& id, const std::string& title, int number, const std::string& url);

    Track(const Track& other) = delete;

    Track& operator=(const Track& other) = delete;

    const std::string getId() const;

    const std::string getName() const;

    int getNumber() const;

    const std::string getUrl() const;

    const Artist& getArtist() const;

    const Album& getAlbum() const;

    void setArtist(const Artist& artist);

    void setAlbum(const Album& album);

private:
    const std::string myId;
    const std::string myName;
    const int myNumber;
    const std::string myUrl;
    const Artist* myArtist = nullptr;
    const Album* myAlbum = nullptr;
};

bool operator==(const Track& lhs, const Track& rhs);

bool operator!=(const Track& lhs, const Track& rhs);

bool operator<(const Track& lhs, const Track& rhs);

bool operator>(const Track& lhs, const Track& rhs);

bool operator<=(const Track& lhs, const Track& rhs);

bool operator>=(const Track& lhs, const Track& rhs);

}



namespace std {

template<>
class hash<domain::Track> {

public:
    size_t operator()(const domain::Track& track) const;
};

}



#endif // TRACK_H
