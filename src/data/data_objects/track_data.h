// track_data.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef TRACKDATA_H
#define TRACKDATA_H



#include <string>
#include <memory>
#include "domain/track.h"



namespace data {

/**
 * @brief Track data read from an external source (such as Ampache server).
 */
class TrackData {

public:

    class NameCompare {
    public:
        bool operator()(const TrackData& lhs, const TrackData& rhs) const;
    };



    /**
     * @brief Constructor.
     *
     * @param id Identifier.
     * @param artistId ID of track's artist.
     * @param albumId ID of tracks's album.
     * @param track Track domain object.
     */
    explicit TrackData(const std::string& id, const std::string& artistId, const std::string& albumId,
        std::unique_ptr<domain::Track> track);

    TrackData(const TrackData& other) = delete;

    TrackData& operator=(const TrackData& other) = delete;

    /**
     * @brief Gets the identifier.
     *
     * @return std::string
     */
    std::string getId() const;

    /**
     * @brief Gets the identifier of track's artist.
     *
     * @return std::string
     */
    std::string getArtistId() const;

    /**
     * @brief Gets the identifier of track's album.
     *
     * @return std::string
     */
    std::string getAlbumId() const;

    /**
     * @brief Gets the track domain object.
     *
     * @return domain::Track&
     */
    domain::Track& getTrack() const;

private:
    // arguments from the constructor
    const std::string myId;
    const std::string myArtistId;
    const std::string myAlbumId;
    const std::unique_ptr<domain::Track> myTrack;
};

bool operator==(const TrackData& lhs, const TrackData& rhs);

bool operator!=(const TrackData& lhs, const TrackData& rhs);

bool operator<(const TrackData& lhs, const TrackData& rhs);

bool operator>(const TrackData& lhs, const TrackData& rhs);

bool operator<=(const TrackData& lhs, const TrackData& rhs);

bool operator>=(const TrackData& lhs, const TrackData& rhs);

}



namespace std {

template<>
class hash<data::TrackData> {

public:
    size_t operator()(const data::TrackData& trackData) const;
};

}



#endif // TRACKDATA_H
