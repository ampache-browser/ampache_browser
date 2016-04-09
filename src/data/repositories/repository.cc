// repository.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <libaudcore/runtime.h>

#include "infrastructure/event/delegate.h"
#include "data/provider_type.h"
#include "data/providers/ampache.h"
#include "data/providers/cache.h"
#include "data/filters/filter.h"
#include "data/indices.h"



namespace data {

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

    AUDDBG("Load from %d, limit %d.\n", offset, limit);
    if (myProviderType == ProviderType::Ampache) {
        myLoadOffset = offset;
        getDataLoadRequestFinishedEvent() += infrastructure::DELEGATE1(
            (&Repository<T, U>::onDataLoadRequestFinished), std::vector<std::unique_ptr<T>>);

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
    // SMELL: ad can be nullptr; in case album data is not found, return null?
    auto dataIter = find_if(myData.begin(), myData.end(),
        [&id](const std::unique_ptr<T>& d) {return d->getId() == id;});
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
    AUDDBG("Setting a filter.\n");
    myIsFilterSet = true;

    myFilter->changed -= infrastructure::DELEGATE0((&Repository<T, U>::onFilterChanged));

    filter->setSourceData(myData);
    filter->changed += infrastructure::DELEGATE0((&Repository<T, U>::onFilterChanged));
    myFilter = move(filter);

    handleFilterSetUnsetOrChanged();
}



template <typename T, typename U>
void Repository<T, U>::unsetFilter() {
    if (!isFiltered()) {
        return;
    }
    AUDDBG("Unsetting a filter.\n");
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
    AUDDBG("Clearing.\n");
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
    AUDDBG("Processing filter changed event.\n");
    handleFilterSetUnsetOrChanged();
}



template <typename T, typename U>
void Repository<T, U>::onDataLoadRequestFinished(std::vector<std::unique_ptr<T>>& data) {
    AUDDBG("Ready %d entries from offset %d.\n", data.size(), myLoadOffset);
    getDataLoadRequestFinishedEvent() -= infrastructure::DELEGATE1((&Repository<T, U>::onDataLoadRequestFinished),
        std::vector<std::unique_ptr<T>>);

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

    if (data.size() == 0) {
        error = true;
        fullyLoaded(error);
        return;
    }

    uint offset = myLoadOffset;
    auto end = offset + data.size();
    if (end > myData.size()) {
        myData.resize(end);
    }

    updateIndices(data);
    for (auto& dataItem: data) {
        handleLoadedItem(*dataItem);

        myData[offset] = std::move(dataItem);
        offset++;
    }

    auto offsetAndLimit = std::pair<int, int>{myLoadOffset, data.size()};
    myUnfilteredFilter->processUpdatedSourceData(myLoadOffset, data.size());
    myFilter->processUpdatedSourceData(myLoadOffset, data.size());
    myLoadOffset = -1;
    myLoadProgress += data.size();
    AUDDBG("Load progress: %d.\n", myLoadProgress);

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

    updateIndices(myData);
    for (auto& data: myData) {
        handleLoadedItem(*data);
    }

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
