// album.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ALBUM_H
#define ALBUM_H


#include <string>
#include <vector>
#include <QtGui/QPixmap>
#include "domain/artist.h"
#include "domain/track.h"

using namespace std;



namespace domain {

class Album {

public:
    Album(const string &id, const string &name, int releaseYear);

    ~Album();

    Album(const Album& other) = default;

    Album& operator=(const Album& other) = default;

    Album(Album&& other) = default;

    Album& operator=(Album&& other) = default;

    // TODO: Define copy and move constructors in other classes that defines destructor.

    const string getId() const;

    const string getName() const;

    int getReleaseYear() const;

    Artist* getArtist() const;

    // TODO: Determine artist from tracks.  Note that Ampache info already contains artist in album data so setArtist
    // might be unnecessary
    void setArtist(Artist* artist);

    QPixmap* getArt() const;

    void setArt(QPixmap* art);

    void addTrack(Track* track);

private:
    const string myId;
    const string myName;
    const int myReleaseYear;
    vector<Track*> myTracks;
    Artist* myArtist;
    QPixmap* myArt;
};

}



#endif // ALBUM_H
