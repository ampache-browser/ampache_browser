// track_repository.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef TRACKREPOSITORY_H
#define TRACKREPOSITORY_H



#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "infrastructure/event/event.h"
#include "../../src/data/data_objects/track_data.h"
#include "../../src/data/data_objects/album_data.h"
#include "filters/filter.h"
#include "filters/unfiltered_filter.h"
#include "domain/track.h"
#include "domain/album.h"
#include "domain/artist.h"



namespace data {

class AmpacheService;
class Cache;
class ArtistRepository;
class AlbumRepository;
class Indices;



/**
 * @brief Stores tracks data, provides means to trigger their load from Ampache server or a cache and updates indices.
 */
class TrackRepository {

public:
    /**
     * @brief Constructor.
     *
     * @param ampacheService Used for communication with Ampache server.
     * @param cache Used for accessing the disk cache.
     * @param artistRepository Used to set track artist.
     * @param albumRepository Used to set album of the track.
     * @param indices Indices to update.
     */
    explicit TrackRepository(AmpacheService& ampacheService, Cache& cache, const ArtistRepository& artistRepository,
        const AlbumRepository& albumRepository, Indices& indices);

    ~TrackRepository();

    TrackRepository(const TrackRepository& other) = delete;

    TrackRepository& operator=(const TrackRepository& other) = delete;

    /**
     * @brief Event fired when some tracks data were loaded.
     *
     * @sa load()
     */
    infrastructure::Event<std::pair<int, int>> loaded{};

    /**
     * @brief Event fired when all tracks data were loaded.
     */
    infrastructure::Event<void> fullyLoaded{};

    /**
     * @brief Event fired when a filter was changed.
     *
     * @sa setFilter(), unsetFilter()
     */
    infrastructure::Event<void> filterChanged{};

    /**
     * @brief Trigger load of tracks data from Ampache server or the cache.
     *
     * @param offset Starting offset.
     * @param limit Number of track data records to load.
     * @return true if load was triggered, false otherwise.
     *
     * @sa ::loaded, ::fullyLoaded
     */
    bool load(int offset, int limit);

    /**
     * @brief Get track at the given offset.
     *
     * @param filteredOffset Offset of the track data record that shall be returned.  It takes filtering into account.
     *        If no filter is set then it is the same as @p offset in the load() function.
     * @return Track at the given @p filteredOffset.
     */
    domain::Track& get(int filteredOffset) const;

    /**
     * @brief Get track with the given ID.
     *
     * @param id The ID of the track that shall be returned.
     * @return Track with the given @p id.
     *
     * @sa domain::Track::getId()
     */
    domain::Track& getById(const std::string& id) const;

    /**
     * @brief Gets the load status of the given track data records.
     *
     * @param filteredOffset Offset of the track data record which load status shall be checked.  It takes
     *        filtering into account.  If no filter is set then it is the same as @p offset in the load() function.
     * @param count Number of track data records to check.
     * @return true if each specified track data record is already loaded.
     */
    bool isLoaded(int filteredOffset, int count = 1) const;

    /**
     * @brief Gets maximal number of track data records.
     *
     * If no filter is set it represents the number of tracks that exists on the Ampache server or cache.  Otherwise
     * it is the number of tracks after applying the filter.
     *
     * @return int
     */
    int maxCount();

    /**
     * @brief Sets a filter.
     *
     * @param filter The filter that shall be set.
     *
     * @sa unsetFilter(), ::filterChanged
     */
    void setFilter(std::unique_ptr<Filter<TrackData>> filter);

    /**
     * @brief Unset the filter.
     *
     * @sa setFilter(), ::filterChanged
     */
    void unsetFilter();

    /**
     * @brief Return the filtering status.
     *
     * @return true if track records are filtered.
     *
     * @sa setFilter(), unsetFilter()
     */
    bool isFiltered() const;

private:
    // the stored tracks data
    std::vector<std::unique_ptr<TrackData>> myTracksData;

    // arguments from the constructor
    AmpacheService& myAmpacheService;
    Cache& myCache;
    const ArtistRepository& myArtistRepository;
    const AlbumRepository& myAlbumRepository;
    Indices& myIndices;

    // number of loaded tracks so far
    int myLoadProgress = 0;

    // starting offset of track records that are being currently loaded; -1 if no track loading is in progress
    int myLoadOffset = -1;

    // filter which is active when no filter is set
    std::shared_ptr<UnfilteredFilter<TrackData>> myUnfilteredFilter = std::shared_ptr<UnfilteredFilter<TrackData>>{
        new UnfilteredFilter<TrackData>{}};

    // the active filter
    std::shared_ptr<Filter<TrackData>> myFilter = nullptr;

    // true if a filter is set (other than myUnfilteredFilter)
    bool myIsFilterSet = false;

    // cached value for maxCount() method
    int myCachedMaxCount = -1;

    void onReadyTracks(std::vector<std::unique_ptr<TrackData>>& trackData);
    void onFilterChanged();

    void loadFromCache();
    void updateIndicies(TrackData& trackData);
    int computeMaxCount() const;
};

}

#endif // TRACKREPOSITORY_H
