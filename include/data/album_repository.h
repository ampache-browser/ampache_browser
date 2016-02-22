// album_repository.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ALBUMREPOSITORY_H
#define ALBUMREPOSITORY_H



#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "infrastructure/event/event.h"
#include "../../src/data/data_objects/album_data.h"
#include "domain/album.h"
#include "domain/artist.h"
#include "filters/filter.h"
#include "filters/unfiltered_filter.h"



namespace data {

class Ampache;
class Cache;
class ArtistRepository;
class Indices;



/**
 * @brief Stores albums data and provides means to trigger their load from Ampache server or a cache.
 */
class AlbumRepository {

public:
    /**
     * @brief Constructor.
     *
     * @param ampache Used for communication with Ampache server.
     * @param cache Used for accessing the disk cache.
     * @param artistRepository Used to set artist of the album.
     * @param indices Indices to update.
     */
    explicit AlbumRepository(Ampache& ampache, Cache& cache, const ArtistRepository& artistRepository,
        Indices& indices);

    ~AlbumRepository();

    AlbumRepository(const AlbumRepository& other) = delete;

    AlbumRepository& operator=(const AlbumRepository& other) = delete;

    /**
     * @brief Event fired when some albums data were loaded.
     *
     * @sa load()
     */
    infrastructure::Event<std::pair<int, int>> loaded{};

    /**
     * @brief Event fired when some album arts were loaded.
     *
     * @sa loadArts()
     */
    infrastructure::Event<std::pair<int, int>> artsLoaded{};

    /**
     * @brief Event fired when all albums data were loaded. This does not include album arts.
     */
    infrastructure::Event<void> fullyLoaded{};

    /**
     * @brief Event fired when a filter was changed.
     *
     * @sa setFilter(), unsetFilter()
     */
    infrastructure::Event<void> filterChanged{};

    /**
     * @brief Trigger load of albums data from Ampache server or the cache.
     *
     * @param offset Starting offset.
     * @param limit Maximum number of album data records to load.
     * @return true if loading was triggered, false otherwise.
     *
     * @sa ::loaded, ::fullyLoaded
     */
    bool load(int offset, int limit);

    /**
     * @brief Get album at the given offset.
     *
     * @param filteredOffset Offset of the album data record that shall be returned.  It takes filtering into account.
     *        If no filter is set then it is the same as @p offset in the load() function.
     * @return Album at the given @p filteredOffset.
     */
    domain::Album& get(int filteredOffset) const;

    /**
     * @brief Get album with the given ID.
     *
     * @param id The ID of the album that shall be returned.
     * @return Album with the given @p id.
     *
     * @sa domain::Album::getId()
     */
    domain::Album& getById(const std::string& id) const;

    /**
     * @brief Get album data with the given ID.
     *
     * @param id The ID of the album data that shall be returned.
     * @return Album data with the given @p id
     *
     * @sa domain::AlbumData::getId()
     */
    AlbumData& getAlbumDataById(const std::string& id) const;

    /**
     * @brief Trigger load of album arts from Ampache server or the cache.
     *
     * @param filteredOffset Starting offset.  It takes filtering into account.  If no filter is set then it is the
     *        same as @p offset in the load() function.
     * @param count Number of album arts to load.
     * @return true if loading was triggered, false otherwise.
     *
     * @sa ::artsLoaded
     */
    bool loadArts(int filteredOffset, int count);

    /**
     * @brief Gets the load status of the given album data records.
     *
     * @param filteredOffset Offset of the album data record which load status shall be checked.  It takes filtering
     *        into account.  If no filter is set then it is the same as @p offset in the load() function.
     * @param count Number of album data records to check.
     * @return true if each specified album data record is already loaded.
     */
    bool isLoaded(int filteredOffset, int count = 1) const;

    /**
     * @brief Gets maximal number of album data records.
     *
     * If no filter is set it represents the number of albums that exists on the Ampache server or cache.  Otherwise
     * it is the number of albums after applying the filter.
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
    void setFilter(std::unique_ptr<Filter<AlbumData>> filter);

    /**
     * @brief Unset the filter.
     *
     * @sa setFilter(), ::filterChanged
     */
    void unsetFilter();

    /**
     * @brief Return the filtering status.
     *
     * @return true if album records are filtered.
     *
     * @sa setFilter(), unsetFilter()
     */
    bool isFiltered() const;

private:
    // the stored albums data
    std::vector<std::unique_ptr<AlbumData>> myAlbumsData;

    // arguments from the constructor
    Ampache& myAmpache;
    Cache& myCache;
    const ArtistRepository& myArtistRepository;
    Indices& myIndices;

    // number of loaded albums so far
    int myLoadProgress = 0;

    // starting offset of album records that are being currently loaded; -1 if no album loading is in progress
    int myLoadOffset = -1;

    // starting offset and number of album arts that are being currently loaded; -1 if no arts loading
    // is in progress
    int myArtsLoadOffset = -1;
    int myArtsLoadCount = -1;

    // filter which is active when no filter is set
    std::shared_ptr<UnfilteredFilter<AlbumData>> myUnfilteredFilter = std::shared_ptr<UnfilteredFilter<AlbumData>>{
        new UnfilteredFilter<AlbumData>{}};

    // the active filter
    std::shared_ptr<Filter<AlbumData>> myFilter = nullptr;

    // true if a filter is set (other than myUnfilteredFilter)
    bool myIsFilterSet = false;

    // cached value for maxCount() method
    int myCachedMaxCount = -1;

    // true if cache is used to load records; false if records are loaded from Ampache server
    bool myCachedLoad = false;

    void onReadyAlbums(std::vector<std::unique_ptr<data::AlbumData>>& albumsData);
    void onAmpacheReadyArts(const std::map<std::string, QPixmap>& arts);
    void onCacheReadyArts(const std::map<std::string, QPixmap>& arts);
    void onFilterChanged();

    void loadFromCache();
    void updateIndices(AlbumData& albumData);
    int computeMaxCount() const;
    domain::Album* findFilteredAlbumById(const std::string& id, int offset, int count) const;
    bool raiseEmptyIfResultNotValid() const;
};

}

#endif // ALBUMREPOSITORY_H
