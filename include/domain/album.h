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



namespace domain {

class Artist;
class Track;



class Album {

public:
    Album(const std::string& id, const std::string& name, int releaseYear);

    const std::string getId() const;

    const std::string getName() const;

    int getReleaseYear() const;

    const Artist& getArtist() const;

    // TODO: Determine artist from tracks.
    void setArtist(const domain::Artist& artist);

    QPixmap* getArt() const;

    void setArt(QPixmap* art);

    void addTrack(Track* track);

private:
    const std::string myId;
    const std::string myName;
    const int myReleaseYear;
    std::vector<Track*> myTracks;
    const Artist* myArtist = nullptr;
    // TODO: change to unique_ptr
    QPixmap* myArt = nullptr;
};

}



#endif // ALBUM_H
