// track_data.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef TRACKDATA_H
#define TRACKDATA_H



#include <string>
#include <memory>
#include "domain/track.h"



namespace data {

class TrackData {

public:
    explicit TrackData(const std::string& id, const std::string& artistId, const std::string& albumId,
        std::unique_ptr<domain::Track> track);

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

}



#endif // TRACKDATA_H
