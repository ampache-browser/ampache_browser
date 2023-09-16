// track.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2023 Róbert Čerňanský



#ifndef TRACK_H
#define TRACK_H



#include <string>



namespace domain {

class Artist;
class Album;



/**
 * @brief Represents the track domain object.
 */
class Track {

public:
    /**
     * @brief Constructor.
     *
     * @param id Identifier.
     * @param title Track name/title.
     * @param disk Designation (usually a number) of a disk on which the track is present in case the album is split
     *             to multiple disks.
     * @param number Number under which the track is listed on the album.
     * @param url URL to media file with recording of the track.
     *
     * @sa getAlbum(), setAlbum()
     */
    Track(const std::string& id, const std::string& title, const std::string& disk, int number, const std::string& url);

    Track(const Track& other) = delete;

    Track& operator=(const Track& other) = delete;

    /**
     * @brief Gets the identifier.
     */
    const std::string getId() const;

    /**
     * @brief Gets track's name/title.
     */
    const std::string getName() const;

    /**
     * @brief Gets the Designation (usually a number) of a disk on which the track is present.
     *
     * If the album of the track is split to multiple disk it typically returns "disk number" or
     * "disk number/total number of disks".  If the album is on one disk only then it typically returns an empty string.
     *
     * @sa getNumber(), getAlbum()
     */
    const std::string getDisk() const;

    /**
     * @brief Gets the number under which the track is listed on the album.
     *
     * @sa getAlbum()
     */
    int getNumber() const;

    /**
     * @brief Gets URL to media file with recording of the track.
     */
    const std::string getUrl() const;

    /**
     * @brief Gets track's artist.
     */
    const Artist* getArtist() const;

    /**
     * @brief Gets track's album.
     *
     * @sa setAlbum(), getDisk(), getNumber()
     */
    const Album* getAlbum() const;

    /**
     * @brief Sets track's artist.
     *
     * @param artist
     */
    void setArtist(const Artist& artist);

    /**
     * @brief Sets track's album.
     *
     * @param album
     *
     * @sa getAlbum()
     */
    void setAlbum(const Album& album);

private:
    // arguments from the constructor
    const std::string myId;
    const std::string myName;
    const std::string myDisk;
    const int myNumber;
    const std::string myUrl;

    // track's artist
    const Artist* myArtist = nullptr;

    // track's album
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
