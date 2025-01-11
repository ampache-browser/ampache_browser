// track_data.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <utility>
#include <string>
#include <memory>

#include "domain/track.h"
#include "track_data.h"

using namespace domain;



namespace data {

bool TrackData::NameCompare::operator()(const TrackData& lhs, const TrackData& rhs) const {
    return lhs.getTrack().getName() < rhs.getTrack().getName();
}



TrackData::TrackData(
    const std::string& id, const std::string& artistId, const std::string& albumId, std::unique_ptr<Track> track):
myId{id},
myArtistId{artistId},
myAlbumId{albumId},
myTrack{std::move(track)} {
}



std::string TrackData::getId() const {
    return myId;
}



std::string TrackData::getArtistId() const {
    return myArtistId;
}



std::string TrackData::getAlbumId() const {
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



size_t std::hash<data::TrackData>::operator()(const data::TrackData& trackData) const {
    return std::hash<string>()(trackData.getId());
}
