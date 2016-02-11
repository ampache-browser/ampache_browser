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



/**
 * @brief Provides fast access to data.
 */
class Indices {

public:
    // SMELL: Could be suboptimal to have only one event for all index types.
    /**
     * @brief Event fired when some of the indices has been changed.
     *
     * @sa updateArtistAlbum(), updateArtistTrack(), updateAlbumTrack()
     */
    infrastructure::Event<void> changed{};

    /**
     * @brief Gets albums data for the given @p artist.
     *
     * @param artist The artist which albums data shall be returned.
     * @return Albums data.
     */
    AlbumDataUnorderedSet& getArtistAlbums(const domain::Artist& artist);

    /**
     * @brief Update album data index for the given @p artist.
     *
     * @param artist The artist which index shall be updated.
     * @param albumData The data which shall be inserted to the index.
     */
    void updateArtistAlbum(const domain::Artist& artist, AlbumData& albumData);

    /**
     * @brief Gets tracks data for the given @p artist.
     *
     * @param artist The artist which tracks data shall be returned.
     * @return Tracks data.
     */
    TrackDataUnorderedSet& getArtistTracks(const domain::Artist& artist);

    /**
     * @brief Update track data index for the given @p artist.
     *
     * @param artist The artist which index shall be updated.
     * @param trackData The data which shall be inserted to the index.
     */
    void updateArtistTrack(const domain::Artist& artist, TrackData& trackData);

    /**
     * @brief Gets tracks data for the given @p album.
     *
     * @param album The album which tracks data shall be returned.
     * @return Tracks data.
     */
    TrackDataUnorderedSet& getAlbumTracks(const domain::Album& album);

    /**
     * @brief Update track data index for the given @p album.
     *
     * @param album The album which index shall be updated.
     * @param trackData The data which shall be inserted to the index.
     */
    void updateAlbumTrack(const domain::Album& album, TrackData& trackData);

private:
    // <artist, albums data> map
    ArtistAlbumIndex myArtistAlbum;

    // <artist, tracks data> map
    ArtistTrackIndex myArtistTrack;

    // <album, tracks data> map
    AlbumTrackIndex myAlbumTrack;
};

}



#endif // INDICES_H
