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
#include <algorithm>

#include "infrastructure/logging/logging.h"
#include "infrastructure/event/delegate.h"
#include "infrastructure/event/event.h"
#include "data/provider_type.h"
#include "data/providers/ampache.h"
#include "data/providers/cache.h"
#include "data/filters/filter.h"
#include "data/filters/unfiltered_filter.h"
#include "data/provider_type.h"
#include "data/indices.h"



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
     * @param filteredOffset Offset of the data item that shall queried for the domain object.  It takes
     *        filtering into account. If no filter is set then it is the same as @p offset in the load() function.
     * @return The domain object of the data item at the given @p filteredOffset.
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
     * @param filteredOffset Starting offset of the data items which load status shall be checked.  It takes filtering
     *        into account.  If no filter is set then it is the same as @p offset in the load() function.
     * @param count Number of data items to check.
     * @return true if each specified data item is already loaded.
     */
    bool isLoaded(int filteredOffset, int count = 1) const;

    /**
     * @brief Gets number of data items (loaded and not loaded).
     *
     * If no filter is set it is the same as maxCount().  Otherwise it is the number of data items after applying
     * the filter.
     *
     * @sa maxCount()
     */
    int count();

    /**
     * @brief Gets maximal number of data items as reported by data provider.
     *
     * The number does not depend on filter.
     *
     * @sa count();
     */
    virtual int maxCount() const = 0;

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
     */
    virtual infrastructure::Event<std::pair<std::vector<std::unique_ptr<T>>, bool>>&
        getDataLoadRequestFinishedEvent() = 0;

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
    virtual void updateIndices(const std::vector<std::unique_ptr<T>>& data) = 0;

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
     * @brief Called when filter has been set, unset or has changed.
     *
     * @sa Filter<T>::changed, setFilter(), unsetFilter(), filterChanged
     */
    virtual void handleFilterSetUnsetOrChanged();

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
    int myCachedCount = -1;

    void onFilterChanged();
    void onDataLoadRequestFinished(std::pair<std::vector<std::unique_ptr<T>>, bool>& dataAndError);

    void loadFromCache();
    int computeCount() const;
};



template <typename T, typename U>
Repository<T, U>::Repository(Ampache& ampache, Cache& cache, Indices& indices):
myAmpache(ampache),
myCache(cache),
myIndices(indices) {
    myUnfilteredFilter->setSourceData(myData);
    myUnfilteredFilter->changed += infrastructure::DELEGATE0((&Repository<T, U>::onFilterChanged));
    myFilter = myUnfilteredFilter;
}



template <typename T, typename U>
Repository<T, U>::~Repository() {
    myUnfilteredFilter->changed -= infrastructure::DELEGATE0((&Repository<T, U>::onFilterChanged));
    if (isFiltered()) {
        myFilter->changed -= infrastructure::DELEGATE0((&Repository<T, U>::onFilterChanged));
    }
}



template <typename T, typename U>
void Repository<T, U>::setProviderType(ProviderType providerType) {
    if (myProviderType != providerType) {
        myProviderType = providerType;
        clear();

        providerChanged();

        if (maxCount() == 0) {
            auto error = false;
            fullyLoaded(error);
        }
    }
}



/**
 * @warning Class does not work correctly if this method is called multiple times for the same data.
 */
template <typename T, typename U>
bool Repository<T, U>::load(int offset, int limit) {
    if (myLoadOffset != -1 || !myLoadingEnabled) {
        return false;
    }

    infrastructure::LOG_DBG("Load from %d, limit %d.", offset, limit);
    if (myProviderType == ProviderType::Ampache) {
        myLoadOffset = offset;
        getDataLoadRequestFinishedEvent() += infrastructure::DELEGATE1(
            (&Repository<T, U>::onDataLoadRequestFinished), std::pair<std::vector<std::unique_ptr<T>>, bool>);

        requestDataLoad(offset, limit);
    } else if (myProviderType == ProviderType::Cache) {
        // SMELL: The condition below is to ignore subsequent requests from model which are not needed since the
        // repository was fully loaded at the first load() call.  This is inconsistent from non-cached loads and it
        // would be better if it was handled by the caller.
        if (myLoadProgress == 0) {
            loadFromCache();
        }
    }
    return true;
}



template <typename T, typename U>
U& Repository<T, U>::get(int filteredOffset) const {
    T& data = myFilter->getFilteredData()[filteredOffset];
    return getDomainObject(data);
}



template <typename T, typename U>
U& Repository<T, U>::getById(const std::string& id) const {
    // SMELL: in case album data is not found, return null?
    auto dataIter = std::find_if(myData.begin(), myData.end(),
        [&id](const std::unique_ptr<T>& d) {return d != nullptr && d->getId() == id;});
    return getDomainObject(**dataIter);
}



template <typename T, typename U>
bool Repository<T, U>::isLoaded(int filteredOffset, int count) const {
    uint end = filteredOffset + count;
    auto filteredData = myFilter->getFilteredData();
    return (filteredData.size() >= end) && all_of(filteredData.begin() + filteredOffset,
        filteredData.begin() + filteredOffset + count, [](const T& fd) {return &fd != nullptr;});
}



template <typename T, typename U>
int Repository<T, U>::count() {
    if (myCachedCount == -1) {
        myCachedCount = computeCount();
    }
    return myCachedCount;
}



template <typename T, typename U>
void Repository<T, U>::disableLoading() {
    myLoadingEnabled = false;
    myCachedCount = -1;
    if (myLoadOffset == -1)
    {
        loadingDisabled();
    }
}



template <typename T, typename U>
void Repository<T, U>::setFilter(std::unique_ptr<Filter<T>> filter) {
    infrastructure::LOG_DBG("Setting a filter.");
    myIsFilterSet = true;

    myFilter->changed -= infrastructure::DELEGATE0((&Repository<T, U>::onFilterChanged));

    filter->setSourceData(myData);
    filter->changed += infrastructure::DELEGATE0((&Repository<T, U>::onFilterChanged));
    myFilter = std::move(filter);

    handleFilterSetUnsetOrChanged();
}



template <typename T, typename U>
void Repository<T, U>::unsetFilter() {
    if (!isFiltered()) {
        return;
    }
    infrastructure::LOG_DBG("Unsetting a filter.");
    myIsFilterSet = false;

    myFilter->changed -= infrastructure::DELEGATE0((&Repository<T, U>::onFilterChanged));

    myUnfilteredFilter->changed += infrastructure::DELEGATE0((&Repository<T, U>::onFilterChanged));
    myFilter = myUnfilteredFilter;

    handleFilterSetUnsetOrChanged();
    filterChanged();
}



template <typename T, typename U>
bool Repository<T, U>::isFiltered() const {
    return myIsFilterSet;
}



template <typename T, typename U>
void Repository<T, U>::handleLoadedItem(const T&) const {
}



template <typename T, typename U>
void Repository<T, U>::clear() {
    infrastructure::LOG_DBG("Clearing.");
    myData.clear();
    myLoadProgress = 0;
    myLoadOffset = -1;

    clearIndices();
    myUnfilteredFilter->processUpdatedSourceData();
    myFilter->processUpdatedSourceData();
}



template <typename T, typename U>
void Repository<T, U>::handleFilterSetUnsetOrChanged() {
    myCachedCount = -1;

    // to the outside world there is no filter to change if not filtered
    if (isFiltered()) {
        filterChanged();
    }
}



template <typename T, typename U>
void Repository<T, U>::onFilterChanged() {
    infrastructure::LOG_DBG("Processing filter changed event.");
    handleFilterSetUnsetOrChanged();
}



template <typename T, typename U>
void Repository<T, U>::onDataLoadRequestFinished(std::pair<std::vector<std::unique_ptr<T>>, bool>& dataAndError) {
    auto data = std::move(dataAndError.first);
    infrastructure::LOG_DBG("Ready %d entries from offset %d.", data.size(), myLoadOffset);
    getDataLoadRequestFinishedEvent() -= infrastructure::DELEGATE1((&Repository<T, U>::onDataLoadRequestFinished),
        std::pair<std::vector<std::unique_ptr<T>>, bool>);

    if (!myLoadingEnabled)
    {
        loadingDisabled();
        return;
    }

    bool error = false;

    // return an empty result if the loaded data are not valid anymore (e. g. due to a provider change)
    if (myLoadOffset == -1) {

        // fire loaded event to give a chance to consumers to continue their processing; even in the case of provider
        // change it might not be necessary since consumers should react on providerChanged event by cancelling
        // of all requests
        auto offsetAndLimit = std::pair<int, int>{0, 0};
        loaded(offsetAndLimit);

        return;
    }

    if (dataAndError.second) {
        error = true;
        fullyLoaded(error);
        return;
    }

    for (auto& dataItem: data) {
        handleLoadedItem(*dataItem);
    }
    updateIndices(data);

    uint offset = myLoadOffset;
    auto end = offset + data.size();
    if (end > myData.size()) {
        myData.resize(end);
    }
    for (auto& dataItem: data) {
        myData[offset] = std::move(dataItem);
        offset++;
    }

    auto offsetAndLimit = std::pair<int, int>{myLoadOffset, data.size()};
    myUnfilteredFilter->processUpdatedSourceData(myLoadOffset, data.size());
    myFilter->processUpdatedSourceData(myLoadOffset, data.size());
    myLoadOffset = -1;
    myLoadProgress += data.size();
    infrastructure::LOG_DBG("Load progress: %d.", myLoadProgress);

    bool isFullyLoaded = myLoadProgress >= maxCount();
    if (isFullyLoaded) {
        saveDataToCache();
    }

    loaded(offsetAndLimit);
    if (isFullyLoaded) {
        fullyLoaded(error);
    }
}



template <typename T, typename U>
void Repository<T, U>::loadFromCache() {
    loadDataFromCache();

    for (auto& data: myData) {
        handleLoadedItem(*data);
    }
    updateIndices(myData);

    myUnfilteredFilter->processUpdatedSourceData();
    myFilter->processUpdatedSourceData();
    myLoadOffset = -1;
    myLoadProgress += myData.size();

    auto offsetAndLimit = std::pair<int, int>{0, myData.size()};
    loaded(offsetAndLimit);
    bool error = false;
    fullyLoaded(error);
}



template <typename T, typename U>
int Repository<T, U>::computeCount() const {
    if (isFiltered() && myLoadProgress != 0) {
        return myFilter->getFilteredData().size();
    }
    return maxCount();
}

}



#endif // REPOSITORY_H
