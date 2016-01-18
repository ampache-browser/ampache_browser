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

bool TrackData::NameCompare::operator()(TrackData& lhs, TrackData& rhs) const {
    return lhs.getTrack().getName() < rhs.getTrack().getName();
}



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



bool operator==(const TrackData& lhs, const TrackData& rhs) {
    return lhs.getId() == rhs.getId();
}



bool operator!=(const TrackData& lhs, const TrackData& rhs) {
    return !operator==(lhs, rhs);
}



bool operator<(const TrackData& lhs, const TrackData& rhs) {
    return (lhs.getId() != rhs.getId()) && (lhs.getTrack() < rhs.getTrack());
}



bool operator>(const TrackData& lhs, const TrackData& rhs) {
    return operator<(rhs, lhs);
}



bool operator<=(const TrackData& lhs, const TrackData& rhs) {
    return !operator>(lhs, rhs);
}



bool operator>=(const TrackData& lhs, const TrackData& rhs) {
    return !operator<(lhs, rhs);
}

}



namespace std {

size_t hash<data::TrackData>::operator()(const data::TrackData& trackData) const {
    return hash<string>()(trackData.getId());
}

}
