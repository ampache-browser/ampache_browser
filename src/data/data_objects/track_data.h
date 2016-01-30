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

class TrackData {

public:

    class NameCompare {
    public:
        bool operator()(const TrackData& lhs, const TrackData& rhs) const;
    };



    explicit TrackData(const std::string& id, const std::string& artistId, const std::string& albumId,
        std::unique_ptr<domain::Track> track);

    TrackData(const TrackData& other) = delete;

    TrackData& operator=(const TrackData& other) = delete;

    std::string getId() const;

    std::string getArtistId() const;

    std::string getAlbumId() const;

    domain::Track& getTrack() const;

private:
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
