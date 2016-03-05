// artist_repository.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <libaudcore/runtime.h>

#include "infrastructure/event/delegate.h"
#include "data/provider_type.h"
#include "data/providers/ampache.h"
#include "data_objects/artist_data.h"
#include "data/indices.h"
#include "data/providers/cache.h"
#include "data/filters/filter.h"
#include "data/artist_repository.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

ArtistRepository::ArtistRepository(Ampache& ampache, Cache& cache, Indices& indices):
myAmpache(ampache),
myCache(cache),
myIndices(indices) {
    myUnfilteredFilter->setSourceData(myArtistsData);
    myUnfilteredFilter->changed += DELEGATE0(&ArtistRepository::onFilterChanged);
    myFilter = myUnfilteredFilter;

    myAmpache.readyArtists += DELEGATE1(&ArtistRepository::onReadyArtists, vector<unique_ptr<ArtistData>>);
}



ArtistRepository::~ArtistRepository() {
    myAmpache.readyArtists -= DELEGATE1(&ArtistRepository::onReadyArtists, vector<unique_ptr<ArtistData>>);
    myUnfilteredFilter->changed -= DELEGATE0(&ArtistRepository::onFilterChanged);
    if (isFiltered()) {
        myFilter->changed -= DELEGATE0(&ArtistRepository::onFilterChanged);
    }
}



void ArtistRepository::setProviderType(ProviderType providerType) {
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



bool ArtistRepository::load(int offset, int limit) {
    if (myLoadOffset != -1 || !myLoadingEnabled) {
        return false;
    }

    AUDDBG("Load from %d, limit %d.\n", offset, limit);
    if (myProviderType == ProviderType::Ampache) {
        myLoadOffset = offset;
        myAmpache.requestArtists(offset, limit);
    } else if (myProviderType == ProviderType::Cache) {
        if (myLoadProgress == 0) {
            loadFromCache();
        }
    }
    return true;
}



Artist& ArtistRepository::get(int filteredOffset) const {
    ArtistData& artistData = myFilter->getFilteredData()[filteredOffset];
    return artistData.getArtist();
}



Artist& ArtistRepository::getById(const string& id) const {
    // SMELL: ad can be nullptr.
    auto artistsDataIter = find_if(myArtistsData.begin(), myArtistsData.end(),
        [&id](const unique_ptr<ArtistData>& ad) {return ad->getId() == id;});
    // TODO: Check whether the artist was really found.  if (artistDataIter == myArtistData.end) { // not found }
    return (*artistsDataIter)->getArtist();
}



bool ArtistRepository::isLoaded(int filteredOffset, int count) const {
    uint end = filteredOffset + count;
    auto filteredArtistsData = myFilter->getFilteredData();
    return (filteredArtistsData.size() >= end) && all_of(filteredArtistsData.begin() + filteredOffset,
        filteredArtistsData.begin() + filteredOffset + count, [](const ArtistData& ad) {return &ad != nullptr;});
}



int ArtistRepository::maxCount() {
    if (myCachedMaxCount == -1) {
        myCachedMaxCount = computeMaxCount();
    }
    return myCachedMaxCount;
}



void ArtistRepository::disableLoading() {
    myLoadingEnabled = false;
    myCachedMaxCount = -1;
    loadingDisabled();
    if (myLoadOffset == -1) {
        auto error = false;
        fullyLoaded(error);
    }
}



void ArtistRepository::setFilter(unique_ptr<Filter<ArtistData>> filter) {
    AUDDBG("Setting a filter.\n");
    myIsFilterSet = true;

    myFilter->changed -= DELEGATE0(&ArtistRepository::onFilterChanged);

    filter->setSourceData(myArtistsData);
    filter->changed += DELEGATE0(&ArtistRepository::onFilterChanged);
    myFilter = move(filter);
    myFilter->apply();
}



void ArtistRepository::unsetFilter() {
    if (!isFiltered()) {
        return;
    }
    AUDDBG("Unsetting a filter.\n");
    myIsFilterSet = false;

    myFilter->changed -= DELEGATE0(&ArtistRepository::onFilterChanged);

    myUnfilteredFilter->changed += DELEGATE0(&ArtistRepository::onFilterChanged);
    myFilter = myUnfilteredFilter;
    myCachedMaxCount = -1;

    filterChanged();
}



bool ArtistRepository::isFiltered() const {
    return myIsFilterSet;
}



void ArtistRepository::onReadyArtists(vector<unique_ptr<ArtistData>>& artistsData) {
    AUDDBG("Ready %d entries from offset %d.\n", artistsData.size(), myLoadOffset);
    bool error = false;

    // return an empty result if the loaded data are not valid anymore (e. g. due to a provider change)
    if (myLoadOffset == -1) {

        // fire loaded event to give a chance to consumers to continue their processing; even in the case of provider
        // change it might not be necessary since consumers should react on providerChanged event by cancelling
        // of all requests
        auto offsetAndLimit = pair<int, int>{0, 0};
        loaded(offsetAndLimit);

        return;
    }

    if (artistsData.size() == 0) {
        error = true;
        fullyLoaded(error);
        return;
    }

    uint offset = myLoadOffset;
    auto end = offset + artistsData.size();
    if (end > myArtistsData.size()) {
        myArtistsData.resize(end);
    }

    for (auto& artistData: artistsData) {
        updateIndices(*artistData);
        myArtistsData[offset] = move(artistData);
        offset++;
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, artistsData.size()};
    myUnfilteredFilter->processUpdatedSourceData(myLoadOffset, artistsData.size());
    myFilter->apply();
    myLoadOffset = -1;
    myLoadProgress += artistsData.size();
    AUDDBG("Load progress: %d.\n", myLoadProgress);

    bool isFullyLoaded = myLoadProgress >= myAmpache.numberOfArtists();
    if (isFullyLoaded) {
        myCache.saveArtistsData(myArtistsData);
    }

    // application can be terminated after loaded event therefore there should be no access to instance variables
    // after it is fired
    loaded(offsetAndLimit);
    if (isFullyLoaded || !myLoadingEnabled) {
        fullyLoaded(error);
    }
}



void ArtistRepository::onFilterChanged() {
    AUDDBG("Processing filter changed event.\n");
    myCachedMaxCount = -1;

    // to the outside world there is no filter to change if not filtered
    if (isFiltered()) {
        filterChanged();
    }
}



void ArtistRepository::clear() {
    AUDDBG("Clearing.\n");
    myArtistsData.clear();
    myLoadProgress = 0;
    myLoadOffset = -1;
    myCachedMaxCount = -1;

    myUnfilteredFilter->processUpdatedSourceData(-1, 0);
    myFilter->apply();
    myIndices.clearArtists();
}



void ArtistRepository::loadFromCache() {
    myArtistsData = myCache.loadArtistsData();

    for (auto& artistData: myArtistsData) {
        updateIndices(*artistData);
    }

    myUnfilteredFilter->processUpdatedSourceData(0, myArtistsData.size());
    myFilter->apply();
    myLoadOffset = -1;
    myLoadProgress += myArtistsData.size();

    // application can be terminated after loaded event therefore there should be no access to instance variables
    // after it is fired
    auto offsetAndLimit = pair<int, int>{0, myArtistsData.size()};
    loaded(offsetAndLimit);
    bool error = false;
    fullyLoaded(error);
}



void ArtistRepository::updateIndices(const ArtistData& artistData) {
    myIndices.addArtist(artistData.getArtist());
}



int ArtistRepository::computeMaxCount() const {
    if (!myLoadingEnabled || (isFiltered() && myLoadProgress != 0)) {
        return myFilter->getFilteredData().size();
    }
    if (myProviderType == ProviderType::Ampache) {
        return myAmpache.numberOfArtists();
    };
    if (myProviderType == ProviderType::Cache) {
        return myCache.numberOfArtists();
    };
    return 0;
}

}
