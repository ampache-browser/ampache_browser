// artist_repository.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ARTISTREPOSITORY_H
#define ARTISTREPOSITORY_H



#include <string>
#include <vector>
#include <memory>

#include "infrastructure/event/event.h"
#include "../../src/data/data_objects/artist_data.h"
#include "filters/filter.h"
#include "filters/unfiltered_filter.h"
#include "domain/artist.h"



namespace data {

class AmpacheService;
class Cache;



/**
 * @brief Stores artists data and provides means to trigger their load from Ampache server or a cache.
 */
class ArtistRepository {

public:
    /**
     * @brief Constructor.
     *
     * @param ampacheService Used for communication with Ampache server.
     * @param cache Used for accessing the disk cache.
     */
    explicit ArtistRepository(AmpacheService& ampacheService, Cache& cache);

    ~ArtistRepository();

    ArtistRepository(const ArtistRepository& other) = delete;

    ArtistRepository& operator=(const ArtistRepository& other) = delete;

    /**
     * @brief Event fired when some artists data were loaded.
     *
     * @sa load()
     */
    infrastructure::Event<std::pair<int, int>> loaded{};

    /**
     * @brief Event fired when all artists data were loaded.
     */
    infrastructure::Event<void> fullyLoaded{};

    /**
     * @brief Event fired when a filter was changed.
     *
     * @sa setFilter(), unsetFilter()
     */
    infrastructure::Event<void> filterChanged{};

    /**
     * @brief Trigger load of artists data from Ampache server or the cache.
     *
     * @param offset Starting offset.
     * @param limit Number of artist data records to load.
     * @return true if load was triggered, false otherwise.
     *
     * @sa ::loaded, ::fullyLoaded
     */
    bool load(int offset, int limit);

    /**
     * @brief Get artist at the given offset.
     *
     * @param filteredOffset Offset of the artist data record that shall be returned.  It takes filtering into account.
     *        If no filter is set then it is the same as @p offset in the load() function.
     * @return Artist at the given @p filteredOffset.
     */
    domain::Artist& get(int filteredOffset) const;

    /**
     * @brief Get artist with the given ID.
     *
     * @param id The ID of the artist that shall be returned.
     * @return Artist with the given @p id.
     *
     * @sa domain::Artist::getId()
     */
    domain::Artist& getById(const std::string& id) const;

    /**
     * @brief Gets the load status of the given artist data records.
     *
     * @param filteredOffset Offset of the artist data record which load status shall be checked.  It takes
     *        filtering into account.  If no filter is set then it is the same as @p offset in the load() function.
     * @param limit Number of artist data records to check.
     * @return true if each specified artist data record is already loaded.
     */
    bool isLoaded(int filteredOffset, int limit = 1) const;

    /**
     * @brief Gets maximal number of artist data records.
     *
     * If no filter is set it represents the number of artists that exists on the Ampache server or cache.  Otherwise
     * it is the number of artists after applying the filter.
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
    void setFilter(std::unique_ptr<Filter<ArtistData>> filter);

    /**
     * @brief Unset the filter.
     *
     * @sa setFilter(), ::filterChanged
     */
    void unsetFilter();

    /**
     * @brief Return the filtering status.
     *
     * @return true if artist records are filtered.
     *
     * @sa setFilter(), unsetFilter()
     */
    bool isFiltered() const;

private:
    // the stored artists data
    std::vector<std::unique_ptr<ArtistData>> myArtistsData;

    // arguments from the constructor
    AmpacheService& myAmpacheService;
    Cache& myCache;

    // number of loaded artists so far
    int myLoadProgress = 0;

    // starting offset of artist records that are being currently loaded; -1 if no artist loading is in progress
    int myLoadOffset = -1;

    // filter which is active when no filter is set
    std::shared_ptr<UnfilteredFilter<ArtistData>> myUnfilteredFilter = std::shared_ptr<UnfilteredFilter<ArtistData>>{
        new UnfilteredFilter<ArtistData>{}};

    // the active filter
    std::shared_ptr<Filter<ArtistData>> myFilter = nullptr;

    // true if a filter is set (other than myUnfilteredFilter)
    bool myIsFilterSet = false;

    // cached value for maxCount() method
    int myCachedMaxCount = -1;

    void onReadyArtists(std::vector<std::unique_ptr<ArtistData>>& artistData);
    void onFilterChanged();

    void loadFromCache();
    int computeMaxCount() const;
};

}

#endif // ARTISTREPOSITORY_H
