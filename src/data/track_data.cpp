// track_data.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <string>
#include <memory>
#include "domain/track.h"
#include "track_data.h"

using namespace std;
using namespace domain;



namespace data {

TrackData::TrackData(const string& id, const string& artistId, const string& albumId, unique_ptr<Track> track):
myId{id},
myArtistId{artistId},
myAlbumId{albumId},
myTrack{move(track)} {
}



string TrackData::getId() const {
    return myId;
}



string TrackData::getArtistId() const {
    return myArtistId;
}



string TrackData::getAlbumId() const {
    return myAlbumId;
}



Track& TrackData::getTrack() const {
    return *myTrack;
}

}
