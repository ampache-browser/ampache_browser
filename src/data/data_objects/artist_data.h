// artist_data.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ARTISTDATA_H
#define ARTISTDATA_H



#include <string>
#include <memory>
#include "domain/artist.h"



namespace data {

/**
 * @brief Artist data read from an external source (such as Ampache server).
 */
class ArtistData {

public:
    /**
     * @brief Constructor.
     *
     * @param id Identifier.
     * @param numberOfAlbums
     * @param numberOfTracks
     * @param artist Artist domain object.
     */
    explicit ArtistData(const std::string& id, int numberOfAlbums, int numberOfTracks,
        std::unique_ptr<domain::Artist> artist);

    ArtistData(const ArtistData& other) = delete;

    ArtistData& operator=(const ArtistData& other) = delete;

    /**
     * @brief Gets the identifier.
     *
     * @return std::string
     */
    std::string getId() const;

    /**
     * @brief Gets number of artist's albums.
     *
     * @return int
     */
    int getNumberOfAlbums() const;

    /**
     * @brief Gets number of artist's tracks.
     *
     * @return int
     */
    int getNumberOfTracks() const;

    /**
     * @brief Gets the artist domain object.
     *
     * @return domain::Artist&
     */
    domain::Artist& getArtist() const;

private:
    // arguments from the constructor
    const std::string myId;
    const int myNumberOfAlbums;
    const int myNumberOfTracks;
    const std::unique_ptr<domain::Artist> myArtist;
};

bool operator==(const ArtistData& lhs, const ArtistData& rhs);

bool operator!=(const ArtistData& lhs, const ArtistData& rhs);

bool operator<(const ArtistData& lhs, const ArtistData& rhs);

bool operator>(const ArtistData& lhs, const ArtistData& rhs);

bool operator<=(const ArtistData& lhs, const ArtistData& rhs);

bool operator>=(const ArtistData& lhs, const ArtistData& rhs);

}



namespace std {

template<>
class hash<data::ArtistData> {

public:
    size_t operator()(const data::ArtistData& artistData) const;
};

}



#endif // ARTISTDATA_H
