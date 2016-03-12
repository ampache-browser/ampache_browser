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

using ArtistAlbumsIndex = std::unordered_map<
    std::reference_wrapper<const domain::Artist>, AlbumDataUnorderedSet,
    std::hash<domain::Artist>>;

using ArtistTracksIndex = std::unordered_map<
    std::reference_wrapper<const domain::Artist>, TrackDataUnorderedSet,
    std::hash<domain::Artist>>;

using AlbumTracksIndex = std::unordered_map<
    std::reference_wrapper<const domain::Album>, TrackDataUnorderedSet,
    std::hash<domain::Album>>;



/**
 * @brief Provides fast access to domain data.
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
     * @brief Adds specified artists to all artist indices.
     *
     * Artists will be added with empty index data.
     *
     * @param artists Artists that shall be added.
     */
    void addArtists(const std::vector<std::reference_wrapper<domain::Artist>>& artists);

    /**
     * @brief Adds specified albums to all album indices.
     *
     * Albums will be added with empty index data.
     *
     * @param albums Albums that shall be added.
     */
    void addAlbums(const std::vector<std::reference_wrapper<domain::Album>>& albums);

    /**
     * @brief Gets albums data for the given @p artist.
     *
     * @param artist The artist which albums data shall be returned.
     * @return Albums data.
     */
    AlbumDataUnorderedSet& getArtistAlbums(const domain::Artist& artist);

    /**
     * @brief Updates artist-albums index by inserting the given data.
     *
     * @param artistAlbums Artists with corresponding albums data which shall be inserted to the index.
     */
    void updateArtistAlbums(const ArtistAlbumsIndex& artistAlbums);

    /**
     * @brief Gets tracks data for the given @p artist.
     *
     * @param artist The artist which tracks data shall be returned.
     * @return Tracks data.
     */
    TrackDataUnorderedSet& getArtistTracks(const domain::Artist& artist);

    /**
     * @brief Updates artist-tracks index by inserting the given data.
     *
     * @param artistTracks Artists with corresponding tracks data which shall be inserted to the index.
     */
    void updateArtistTracks(const ArtistTracksIndex& artistTracks);

    /**
     * @brief Gets tracks data for the given @p album.
     *
     * @param album The album which tracks data shall be returned.
     * @return Tracks data.
     */
    TrackDataUnorderedSet& getAlbumTracks(const domain::Album& album);

    /**
     * @brief Updates album-tracks index by inserting the given data.
     *
     * @param albumTracks Albums with corresponding tracks data which shall be inserted to the index.
     */
    void updateAlbumTracks(const AlbumTracksIndex& albumTracks);

    /**
     * @brief Removes all artist indices.
     *
     * @sa addArtist()
     */
    void clearArtists();

     /**
     * @brief Removes all album indices.
     *
     * @sa addAlbum()
     */
    void clearAlbums();

    /**
     * @brief Removes albums from each artist.
     *
     * @sa updateArtistAlbums()
     */
    void clearArtistsAlbums();

    /**
     * @brief Removes tracks from each artist.
     *
     * @sa updateArtistTracks()
     */
    void clearArtistsTracks();

    /**
     * @brief Removes tracks from each album.
     *
     * @sa updateAlbumTracks()
     */
    void clearAlbumsTracks();

private:
    // <artist, albums data> map
    ArtistAlbumsIndex myArtistAlbums;

    // <artist, tracks data> map
    ArtistTracksIndex myArtistTracks;

    // <album, tracks data> map
    AlbumTracksIndex myAlbumTracks;
};

}



#endif // INDICES_H
