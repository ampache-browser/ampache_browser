// artist_repository.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <functional>

#include "infrastructure/event/delegate.h"
#include "data/providers/ampache_service.h"
#include "data_objects/artist_data.h"
#include "data/providers/cache.h"
#include "data/filters/filter.h"
#include "data/artist_repository.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

ArtistRepository::ArtistRepository(AmpacheService& ampacheService, Cache& cache):
myAmpacheService(ampacheService),
myCache(cache) {
    myUnfilteredFilter->setSourceData(myArtistsData);
    myUnfilteredFilter->changed += DELEGATE0(&ArtistRepository::onFilterChanged);
    myFilter = myUnfilteredFilter;

    myAmpacheService.readyArtists += DELEGATE1(&ArtistRepository::onReadyArtists, vector<unique_ptr<ArtistData>>);
}



ArtistRepository::~ArtistRepository() {
    myAmpacheService.readyArtists -= DELEGATE1(&ArtistRepository::onReadyArtists, vector<unique_ptr<ArtistData>>);
    myUnfilteredFilter->changed -= DELEGATE0(&ArtistRepository::onFilterChanged);
    if (isFiltered()) {
        myFilter->changed -= DELEGATE0(&ArtistRepository::onFilterChanged);
    }
}



bool ArtistRepository::load(int offset, int limit) {
    if (myLoadOffset != -1) {
        return false;
    }

    if (!myAmpacheService.getIsConnected() || (myCache.getLastUpdate() > myAmpacheService.getLastUpdate())) {
        if (myLoadProgress == 0) {
            loadFromCache();
        }
    } else {
        myLoadOffset = offset;
        myAmpacheService.requestArtists(offset, limit);
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



bool ArtistRepository::isLoaded(int filteredOffset, int limit) const {
    uint end = filteredOffset + limit;
    auto filteredArtistsData = myFilter->getFilteredData();
    return (filteredArtistsData.size() >= end) && all_of(filteredArtistsData.begin() + filteredOffset,
        filteredArtistsData.begin() + filteredOffset + limit, [](const ArtistData& ad) {return &ad != nullptr;});
}



int ArtistRepository::maxCount() {
    if (myCachedMaxCount == -1) {
        myCachedMaxCount = computeMaxCount();
    }
    return myCachedMaxCount;
}



void ArtistRepository::setFilter(unique_ptr<Filter<ArtistData>> filter) {
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
    uint offset = myLoadOffset;
    auto end = offset + artistsData.size();
    if (end > myArtistsData.size()) {
        myArtistsData.resize(end);
    }

    for (auto& artistData: artistsData) {
        myArtistsData[offset] = move(artistData);
        offset++;
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, artistsData.size()};
    myUnfilteredFilter->processUpdatedSourceData(myLoadOffset, artistsData.size());
    myFilter->apply();
    myLoadOffset = -1;
    loaded(offsetAndLimit);

    myLoadProgress += artistsData.size();
    if (myLoadProgress >= myAmpacheService.numberOfArtists()) {
        myCache.saveArtistsData(myArtistsData);
        fullyLoaded();
    }
}



void ArtistRepository::onFilterChanged() {
    myCachedMaxCount = -1;

    // to the outside world there is no filter to change if not filtered
    if (isFiltered()) {
        filterChanged();
    }
}



void ArtistRepository::loadFromCache() {
    myArtistsData = myCache.loadArtistsData();

    myUnfilteredFilter->processUpdatedSourceData(0, myArtistsData.size());
    myFilter->apply();
    myLoadOffset = -1;
    auto offsetAndLimit = pair<int, int>{0, myArtistsData.size()};
    loaded(offsetAndLimit);

    myLoadProgress += myArtistsData.size();
    fullyLoaded();
}



int ArtistRepository::computeMaxCount() const {
    if (myIsFilterSet && myLoadProgress != 0) {
        return myFilter->getFilteredData().size();
    }
    return myAmpacheService.getIsConnected() ? myAmpacheService.numberOfArtists() : myCache.numberOfArtists();
}

}
