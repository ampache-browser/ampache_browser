// album.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ALBUM_H
#define ALBUM_H



#include <string>
#include <deque>
#include <memory>
#include <QtGui/QPixmap>



namespace domain {

class Artist;
class Track;



class Album {

public:
    Album(const std::string& id, const std::string& name, int releaseYear);

    Album(const Album& other) = delete;

    Album& operator=(const Album& other) = delete;

    const std::string getId() const;

    const std::string getName() const;

    int getReleaseYear() const;

    const Artist& getArtist() const;

    // TODO: Determine artist from tracks.
    void setArtist(const Artist& artist);

    bool hasArt() const;

    QPixmap& getArt() const;

    void setArt(std::unique_ptr<QPixmap> art);

    void addTrack(const Track& track);

private:
    const std::string myId;
    const std::string myName;
    const int myReleaseYear;
    std::deque<const Track*> myTracks;
    const Artist* myArtist = nullptr;
    std::unique_ptr<QPixmap> myArt = nullptr;
};

}



#endif // ALBUM_H
