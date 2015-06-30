// artist_repository.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



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



Artist& ArtistRepository::get(int offset) const {
    return myArtistsData[offset]->getArtist();
}



Artist& ArtistRepository::getById(const string& id) const {
    auto artistDataIter = find_if(myArtistsData.begin(), myArtistsData.end(),
        [id](const unique_ptr<ArtistData>& ad) {return ad->getId() == id;});
    return (*artistDataIter)->getArtist();
}



bool ArtistRepository::isLoaded(int offset, int limit) const {
    uint end = offset + limit;
    return (myArtistsData.size() >= end) && all_of(myArtistsData.begin() + offset,
        myArtistsData.begin() + offset + limit, [](const unique_ptr<ArtistData>& ad) {return ad != nullptr;});
}



int ArtistRepository::maxCount() const {
    return myAmpacheService.numberOfArtists();
}



void ArtistRepository::onReadyArtists(vector<unique_ptr<ArtistData>>& artistsData) {
    uint offset = myLoadOffset;
    auto end = offset + artistsData.size();
    if (end > myArtistsData.size()) {
        myArtistsData.resize(end);
    }

    for (auto& artistData: artistsData) {
        myArtistsData[offset++] = move(artistData);
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, artistsData.size()};
    myLoadOffset = -1;
    loaded(offsetAndLimit);
}

}
