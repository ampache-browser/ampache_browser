// indices.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



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

using AlbumDataUnorderedSet = std::unordered_set<std::reference_wrapper<AlbumData>, std::hash<AlbumData>>;

using TrackDataUnorderedSet = std::unordered_set<std::reference_wrapper<TrackData>, std::hash<TrackData>>;

using ArtistAlbumIndex = std::unordered_map<
    std::reference_wrapper<const domain::Artist>, AlbumDataUnorderedSet,
    std::hash<domain::Artist>>;

using ArtistTrackIndex = std::unordered_map<
    std::reference_wrapper<const domain::Artist>, TrackDataUnorderedSet,
    std::hash<domain::Artist>>;

using AlbumTrackIndex = std::unordered_map<
    std::reference_wrapper<const domain::Album>, TrackDataUnorderedSet,
    std::hash<domain::Album>>;



class Indices {

public:
    // SMELL: Could be suboptimal to have only one event for all index types.
    infrastructure::Event<void> changed{};

    AlbumDataUnorderedSet& getArtistAlbum(const domain::Artist& artist);

    void updateArtistAlbum(const domain::Artist& artist, AlbumData& albumData);

    TrackDataUnorderedSet& getArtistTrack(const domain::Artist& artist);

    void updateArtistTrack(const domain::Artist& artist, TrackData& trackData);

    TrackDataUnorderedSet& getAlbumTrack(const domain::Album& album);

    void updateAlbumTrack(const domain::Album& album, TrackData& trackData);

private:
    ArtistAlbumIndex myArtistAlbum;

    ArtistTrackIndex myArtistTrack;

    AlbumTrackIndex myAlbumTrack;
};

}



#endif // INDICES_H
