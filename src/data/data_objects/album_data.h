// album_data.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ALBUMDATA_H
#define ALBUMDATA_H



#include <string>
#include <memory>
#include "domain/album.h"



namespace data {

/**
 * @brief Album data read from an external source (such as Ampache server).
 */
class AlbumData {

public:
    /**
     * @brief Constructor.
     *
     * @param id Identifier.
     * @param artUrl URL of the album art (cover).
     * @param artistId ID of album's artist.
     * @param numberOfTracks
     * @param album Album domain object.
     */
    explicit AlbumData(const std::string& id, const std::string& artUrl, const std::string& artistId,
        int numberOfTracks, std::unique_ptr<domain::Album> album);

    AlbumData(const AlbumData& other) = delete;

    AlbumData& operator=(const AlbumData& other) = delete;

    /**
     * @brief Gets the identifier.
     *
     * @return std::string
     */
    std::string getId() const;

    /**
     * @brief Gets URL of the album art (cover).
     *
     * @return std::string
     */
    std::string getArtUrl() const;

    /**
     * @brief Gets the identifier of album's artist.
     *
     * @return std::string
     */
    std::string getArtistId() const;

    /**
     * @brief Gets number of tracks on the album.
     *
     * @return int
     */
    int getNumberOfTracks() const;

    /**
     * @brief Gets the album domain object.
     *
     * @return domain::Album&
     */
    domain::Album& getAlbum() const;

    /**
     * @brief Returns true if the album has artist.
     *
     * @return bool
     */
    bool hasArtist() const;

private:
    // arguments from the constructor
    const std::string myId;
    const std::string myArtUrl;
    const std::string myArtistId;
    const int myNumberOfTracks;
    const std::unique_ptr<domain::Album> myAlbum;
};

bool operator==(const AlbumData& lhs, const AlbumData& rhs);

bool operator!=(const AlbumData& lhs, const AlbumData& rhs);

bool operator<(const AlbumData& lhs, const AlbumData& rhs);

bool operator>(const AlbumData& lhs, const AlbumData& rhs);

bool operator<=(const AlbumData& lhs, const AlbumData& rhs);

bool operator>=(const AlbumData& lhs, const AlbumData& rhs);

}



namespace std {

template<>
class hash<data::AlbumData> {

public:
    size_t operator()(const data::AlbumData& albumData) const;
};

}



#endif // ALBUMDATA_H
