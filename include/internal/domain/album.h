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



/**
 * @brief Represents the album domain object.
 */
class Album {

public:
    /**
     * @brief Constructor.
     *
     * @param id Identifier.
     * @param name Album's name/title.
     * @param releaseYear Year of the album's release.
     * @param mediaNumber Media number (some albums can be on multiple media such as CDs or DVDs).
     */
    Album(const std::string& id, const std::string& name, int releaseYear, int mediaNumber);

    Album(const Album& other) = delete;

    Album& operator=(const Album& other) = delete;

    /**
     * @brief Gets the identifier.
     *
     * @return const std::string
     */
    const std::string getId() const;

    /**
     * @brief Gets album's name/title.
     *
     * @return const std::string
     */
    const std::string getName() const;

    /**
     * @brief Gets year of the album's release.
     *
     * @return int
     */
    int getReleaseYear() const;

    /**
     * @brief Gets media number.
     *
     * Some albums can be on multiple media such as CDs or DVDs.  This denotes the media which this part of the album
     * is.
     *
     * @return int
     */
    int getMediaNumber() const;

    /**
     * @brief Gets album's artist.
     *
     * @return const domain::Artist&
     */
    const Artist& getArtist() const;

    /**
     * @brief Sets album's artist.
     *
     * @param artist
     */
    void setArtist(const Artist& artist);

    /**
     * @brief Returns true if this instance has an album art.
     *
     * @return bool
     *
     * @sa getArt(), setArt()
     */
    bool hasArt() const;

    /**
     * @brief Gets the album's art (cover).
     *
     * @return QPixmap&
     *
     * @sa hasArt(), setArt()
     */
    QPixmap& getArt() const;

    /**
     * @brief Sets the almum's art (cover).
     *
     * @param art Album art image.
     *
     * @sa hasArt(), getArt()
     */
    void setArt(std::unique_ptr<QPixmap> art);

private:
    // arguments from the constructor
    const std::string myId;
    const std::string myName;
    const int myReleaseYear;
    const int myMediaNumber;

    // album's artist
    const Artist* myArtist = nullptr;

    // album's art (cover)
    std::unique_ptr<QPixmap> myArt = nullptr;
};

bool operator==(const Album& lhs, const Album& rhs);

bool operator!=(const Album& lhs, const Album& rhs);

bool operator<(const Album& lhs, const Album& rhs);

bool operator>(const Album& lhs, const Album& rhs);

bool operator<=(const Album& lhs, const Album& rhs);

bool operator>=(const Album& lhs, const Album& rhs);

}



namespace std {

template<>
class hash<domain::Album> {

public:
    size_t operator()(const domain::Album& album) const;
};

}



#endif // ALBUM_H
