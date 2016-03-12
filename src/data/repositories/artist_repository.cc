// artist_repository.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <libaudcore/runtime.h>

#include "domain/artist.h"
#include "infrastructure/event/delegate.h"
#include "data/provider_type.h"
#include "data/providers/ampache.h"
#include "data/providers/cache.h"
#include "../data_objects/artist_data.h"
#include "data/indices.h"
#include "data/repositories/artist_repository.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

ArtistRepository::ArtistRepository(Ampache& ampache, Cache& cache, Indices& indices):
Repository<ArtistData, Artist>(ampache, cache, indices) {
}



void ArtistRepository::requestDataLoad(int offset, int limit) {
    myAmpache.requestArtists(offset, limit);
}



Artist& ArtistRepository::getDomainObject(const data::ArtistData& dataItem) const {
    return dataItem.getArtist();
}



Event<vector<unique_ptr<ArtistData>>>& ArtistRepository::getDataLoadRequestFinishedEvent() {
    return myAmpache.readyArtists;
}



void ArtistRepository::loadDataFromCache() {
    myData = myCache.loadArtistsData();
}



void ArtistRepository::saveDataToCache() {
    myCache.saveArtistsData(myData);
}



void ArtistRepository::updateIndices(const vector<unique_ptr<ArtistData>>& data) {
    vector<reference_wrapper<Artist>> artists;
    for (auto& dataItem: data) {
        artists.push_back(dataItem->getArtist());
    }
    myIndices.addArtists(artists);
}



void ArtistRepository::clearIndices() {
    myIndices.clearArtists();
}



int ArtistRepository::getMaxDataSize() const {
    if (myProviderType == ProviderType::Ampache) {
        return myAmpache.numberOfArtists();
    };
    if (myProviderType == ProviderType::Cache) {
        return myCache.numberOfArtists();
    };
    return 0;
}

}
