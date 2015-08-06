// artist_repository.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <functional>

#include "data/ampache_service.h"
#include "artist_data.h"
#include "data/artist_repository.h"

using namespace std;
using namespace placeholders;
using namespace domain;



namespace data {

ArtistRepository::ArtistRepository(AmpacheService& ampacheService):
myAmpacheService(ampacheService) {
    myAmpacheService.readyArtists += bind(&ArtistRepository::onReadyArtists, this, _1);
}



bool ArtistRepository::load(int offset, int limit) {
    if (myLoadOffset != -1) {
        return false;
    }
    myLoadOffset = offset;
    myAmpacheService.requestArtists(offset, limit);
    return true;
}



Artist& ArtistRepository::get(int filteredOffset) const {
    ArtistData& artistData = myArtistDataReferences[filteredOffset];
    return artistData.getArtist();
}



Artist& ArtistRepository::getById(const string& id) const {
    return getArtistDataById(id).getArtist();
}



ArtistData& ArtistRepository::getArtistDataById(const string& id) const {
    auto artistsDataIter = find_if(myArtistsData.begin(), myArtistsData.end(),
        [&id](const unique_ptr<ArtistData>& ad) {return ad->getId() == id;});
    // TODO: Check whether the artist was really found.
    return **artistsDataIter;
}



vector<reference_wrapper<Artist>> ArtistRepository::getAll() const {
    vector<reference_wrapper<Artist>> artists;
    for (auto& artistData: myArtistsData) {
        artists.push_back(artistData->getArtist());
    }
    return artists;
}



bool ArtistRepository::isLoaded(int filteredOffset, int limit) const {
    uint end = filteredOffset + limit;
    return (myArtistDataReferences.size() >= end) && all_of(myArtistDataReferences.begin() + filteredOffset,
        myArtistDataReferences.begin() + filteredOffset + limit, [](const ArtistData& ad) {return &ad != nullptr;});
}



int ArtistRepository::maxCount() {
    if (myCachedMaxCount == -1) {
        myCachedMaxCount = computeMaxCount();
    }
    return myCachedMaxCount;
}



/**
 * @warning May be called no sooner than after the repository is fully loaded.
 */
void ArtistRepository::setNameFilter(const string& namePattern) {
    unsetFilter();
    myIsFilterSet = true;
    vector<reference_wrapper<ArtistData>> filteredArtistData;
    for (auto& artistData: myArtistsData) {
        auto name = artistData->getArtist().getName();
        if (search(name.begin(), name.end(), namePattern.begin(), namePattern.end(),
            [](char c1, char c2) {return toupper(c1) == toupper(c2); }) != name.end()) {

            filteredArtistData.push_back(*artistData);
        }
    }
    myArtistDataReferences.swap(myStoredArtistDataReferences);
    myArtistDataReferences.swap(filteredArtistData);

    myCachedMaxCount = -1;

    bool b = false;
    filterChanged(b);
}



void ArtistRepository::unsetFilter() {
    if (!myIsFilterSet) {
        return;
    }
    myArtistDataReferences.clear();
    myArtistDataReferences.swap(myStoredArtistDataReferences);
    myIsFilterSet = false;
    myCachedMaxCount = -1;

    bool b = false;
    filterChanged(b);
}



void ArtistRepository::onReadyArtists(vector<unique_ptr<ArtistData>>& artistsData) {
    uint offset = myLoadOffset;
    auto end = offset + artistsData.size();
    if (end > myArtistsData.size()) {
        myArtistsData.resize(end);

        // resize references container
        for (auto idx = myArtistDataReferences.size(); idx < end; idx++) {
            myArtistDataReferences.push_back(*myArtistsData[idx]);
        }
    }

    for (auto& artistData: artistsData) {
        myArtistDataReferences[offset] = *artistData;
        myArtistsData[offset] = move(artistData);
        offset++;
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, artistsData.size()};
    myLoadOffset = -1;
    myCachedMaxCount = -1;
    loaded(offsetAndLimit);

    myLoadProgress += artistsData.size();
    if (myLoadProgress >= myAmpacheService.numberOfArtists()) {
        bool b = false;
        fullyLoaded(b);
    }
}



int ArtistRepository::computeMaxCount() const {
    if (myIsFilterSet) {
        return myArtistDataReferences.size();
    }
    return myAmpacheService.numberOfArtists();
}

}
