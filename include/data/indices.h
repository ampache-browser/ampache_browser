// indices.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef INDICES_H
#define INDICES_H



#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "infrastructure/event/event.h"
#include "domain/artist.h"
#include "domain/album.h"
#include "../../src/data/data_objects/album_data.h"
#include "../../src/data/data_objects/track_data.h"



namespace data {

using ArtistAlbumIndex = std::unordered_map<
    std::reference_wrapper<const domain::Artist>,
    std::unordered_set<std::reference_wrapper<AlbumData>, std::hash<AlbumData>>,
    std::hash<domain::Artist>>;

using ArtistTrackIndex = std::unordered_map<
    std::reference_wrapper<const domain::Artist>,
    std::unordered_set<std::reference_wrapper<TrackData>, std::hash<TrackData>>,
    std::hash<domain::Artist>>;

using AlbumTrackIndex = std::unordered_map<
    std::reference_wrapper<const domain::Album>,
    std::unordered_set<std::reference_wrapper<TrackData>, std::hash<TrackData>>,
    std::hash<domain::Album>>;



class Indices {

public:
    // SMELL: Could be suboptimal to have only one event for all index types.
    infrastructure::Event<void> changed{};

    ArtistAlbumIndex& getArtistAlbum();

    void updateArtistAlbum(domain::Artist& artist, AlbumData& albumData);

    ArtistTrackIndex& getArtistTrack();

    void updateArtistTrack(domain::Artist& artist, TrackData& trackData);

    AlbumTrackIndex& getAlbumTrack();

    void updateAlbumTrack(domain::Album& album, TrackData& trackData);

private:
    ArtistAlbumIndex myArtistAlbum;

    ArtistTrackIndex myArtistTrack;

    AlbumTrackIndex myAlbumTrack;
};

}



#endif // INDICES_H
