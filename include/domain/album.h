// album.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ALBUM_H
#define ALBUM_H



class Artist;
class Track;

#include <string>
#include <vector>
#include <QtGui/QPixmap>



namespace domain {

class Album {

public:
    Album(const std::string id, const std::string name, int releaseYear);

    ~Album();

    Album(const Album& other) = default;

    Album& operator=(const Album& other) = default;

    Album(Album&& other) = default;

    Album& operator=(Album&& other) = default;

    // TODO: Define copy and move constructors in other classes that defines destructor.

    const std::string getId() const;

    const std::string getName() const;

    int getReleaseYear() const;

    Artist* getArtist() const;

    // TODO: Determine artist from tracks.  Note that Ampache info already contains artist in album data so setArtist
    // might be unnecessary
    void setArtist(Artist* artist);

    QPixmap* getArt() const;

    void setArt(QPixmap* art);

    void addTrack(Track* track);

private:
    const std::string myId;
    const std::string myName;
    const int myReleaseYear;
    std::vector<Track*> myTracks;
    Artist* myArtist;
    QPixmap* myArt;
};

}



#endif // ALBUM_H
