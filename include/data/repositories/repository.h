// repository.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef REPOSITORY_H
#define REPOSITORY_H



#include <string>
#include <vector>

#include "infrastructure/event/event.h"
#include "data/filters/filter.h"
#include "data/filters/unfiltered_filter.h"
#include "data/provider_type.h"



namespace data {

class Ampache;
class Cache;
class Indices;



/**
 * @brief Base class for a repository.
 *
 * Stores data, provides means to trigger their load from Ampache server or a cache; supports indices update.
 */
template <typename T, typename U>
class Repository {

public:
    /**
     * @brief Constructor.
     *
     * @param ampache Used for communication with Ampache server.
     * @param cache Used for accessing the disk cache.
     * @param indices Indices that shall be updated.
     */
    explicit Repository(Ampache& ampache, Cache& cache, Indices& indices);

    virtual ~Repository();

    Repository(const Repository& other) = delete;

    Repository& operator=(const Repository& other) = delete;

    /**
     * @brief Event fired when some data were loaded.
     *
     * @sa load()
     */
    infrastructure::Event<std::pair<int, int>> loaded{};

    /**
     * @brief Event fired when all data were loaded.
     */
    infrastructure::Event<bool> fullyLoaded{};

    /**
     * @brief Event fired when a filter was changed.
     *
     * @sa setFilter(), unsetFilter()
     */
    infrastructure::Event<void> filterChanged{};

    /**
     * @brief Event fired when further loading was disabled.
     *
     * @sa disableLoading()
     */
    infrastructure::Event<void> loadingDisabled{};

    /**
     * @brief Event fired when a data provider has been changed.
     *
     * @sa setProviderType()
     */
    infrastructure::Event<void> providerChanged{};

    /**
     * @brief Sets the which provider should be used to load data.
     *
     * @note maxCount() can change when setting the provider type.
     *
     * @param providerType The type of provider that shall be used to load data.
     *
     * @sa ::providerChanged
     */
    virtual void setProviderType(ProviderType providerType);

    /**
     * @brief Trigger load of data from Ampache server or the cache.
     *
     * @param offset Starting offset.
     * @param limit Maximal number of data items to load.
     * @return true if load was triggered, false otherwise.
     *
     * @sa ::loaded, ::fullyLoaded
     */
    bool load(int offset, int limit);

    /**
     * @brief Get a domain object of the data item at the given offset.
     *
     * @param filteredOffset Offset of the data item that shall be returned.  It takes filtering into account.
     *        If no filter is set then it is the same as @p offset in the load() function.
     * @return The domain object of the  data item at the given @p filteredOffset.
     */
    U& get(int filteredOffset) const;

    /**
     * @brief Get a domain object of the data item with the given ID.
     *
     * @param id The ID of the data item which domain object shall be returned.
     * @return The domain object of the data item with the given @p id.
     */
    U& getById(const std::string& id) const;

    /**
     * @brief Gets the load status of the given data items.
     *
     * @param filteredOffset Offset of the data item which load status shall be checked.  It takes filtering into
     *        account.  If no filter is set then it is the same as @p offset in the load() function.
     * @param count Number of data items to check.
     * @return true if each specified data item is already loaded.
     */
    bool isLoaded(int filteredOffset, int count = 1) const;

    /**
     * @brief Gets maximal number of data items.
     *
     * If no filter is set it represents the number of data items that exists on the Ampache server or cache.  Otherwise
     * it is the number of data items after applying the filter.
     *
     * @return int
     */
    int maxCount();

    /**
     * @brief Disables furher loading.
     *
     * @sa ::loadingDisabled
     */
    virtual void disableLoading();

    /**
     * @brief Sets a filter.
     *
     * @param filter The filter that shall be set.
     *
     * @sa unsetFilter(), ::filterChanged
     */
    void setFilter(std::unique_ptr<Filter<T>> filter);

    /**
     * @brief Unset the filter.
     *
     * @sa setFilter(), ::filterChanged
     */
    void unsetFilter();

    /**
     * @brief Return the filtering status.
     *
     * @return true if the data are filtered.
     *
     * @sa setFilter(), unsetFilter()
     */
    bool isFiltered() const;

protected:
    /**
     * @brief The stored data.
     */
    std::vector<std::unique_ptr<T>> myData;

    /**
     * @brief Accesses the Ampache server.
     */
    Ampache& myAmpache;

    /**
     * @brief Accesses the cache.
     */
    Cache& myCache;

    /**
     * @brief Indices for updating.
     */
    Indices& myIndices;

    /**
     * @brief Loading is enablement status.
     */
    bool myLoadingEnabled = true;

    /**
     * @brief Currently used data provider type.
     */
    ProviderType myProviderType = ProviderType::None;

    /**
     * @brief The active filter.
     */
    std::shared_ptr<Filter<T>> myFilter = nullptr;

    /**
     * @brief Called when current filter has changed.
     *
     * @sa Filter<T>::changed
     */
    virtual void onFilterChanged();

    /**
     * @brief Requests data loading from Ampache.
     *
     * @param offset Starting offset.
     * @param limit Maximal number of data items to load.
     *
     * @sa ::myAmpache
     */
    virtual void requestDataLoad(int offset, int limit) = 0;

    /**
     * @brief Returns domain object from the given @p dataItem.
     */
    virtual U& getDomainObject(const T& dataItem) const = 0;

    /**
     * @brief Returns event that is fired when load request has finished.
     *
     * @sa requestDataLoad
     *
     * @return infrastructure::Event<std::vector<std::unique_ptr<T>>>&
     */
    virtual infrastructure::Event<std::vector<std::unique_ptr<T>>>& getDataLoadRequestFinishedEvent() = 0;

    /**
     * @brief Loads data from cache.
     *
     * @sa ::myData, ::myCache
     */
    virtual void loadDataFromCache() = 0;

    /**
     * @brief Saves loaded data to cache.
     *
     * @sa ::myData, ::myCache
     */
    virtual void saveDataToCache() = 0;

    /**
     * @brief Called when a data item is loaded.
     *
     * @param dataItem The loaded data item.
     */
    virtual void handleLoadedItem(const T& dataItem) const;

    /**
     * @brief Updates indices for the given @p dataItem.
     *
     * @sa ::myIndices
     */
    virtual void updateIndices(T& dataItem) = 0;

    /**
     * @brief Clears the repository as if no data were loaded.
     */
    virtual void clear();

    /**
     * @brief Clears indices.
     *
     * @sa ::myIndices
     */
    virtual void clearIndices() = 0;

    /**
     * @brief Returns maximal number of data items that can be loaded.
     *
     * @return Number of data items from server or cache.  0 if ProviderType::None is set.
     */
    virtual int getMaxDataSize() const = 0;

private:
    // number of loaded data items so far
    int myLoadProgress = 0;

    // starting offset of data items that are being currently loaded; -1 if no loading is in progress
    int myLoadOffset = -1;

    // filter which is active when no filter is set
    std::shared_ptr<UnfilteredFilter<T>> myUnfilteredFilter = std::shared_ptr<UnfilteredFilter<T>>{
        new UnfilteredFilter<T>{}};

    // true if a filter is set (other than myUnfilteredFilter)
    bool myIsFilterSet = false;

    // cached value for maxCount() method
    int myCachedMaxCount = -1;

    void onDataLoadRequestFinished(std::vector<std::unique_ptr<T>>& data);

    void loadFromCache();
    int computeMaxCount() const;
};

}



#include "src/data/repositories/repository.cc"



#endif // REPOSITORY_H
