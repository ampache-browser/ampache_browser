// album_repository.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <iterator>
#include <unordered_set>

#include "domain/artist.h"
#include "data/ampache_service.h"
#include "album_data.h"
#include "index_types.h"
#include "data/artist_repository.h"
#include "data/album_repository.h"

using namespace std;
using namespace placeholders;
using namespace domain;



namespace data {

AlbumRepository::AlbumRepository(AmpacheService& ampacheService, ArtistRepository& artistRepository):
myAmpacheService(ampacheService),
myArtistRepository(artistRepository) {
    // SMELL: Should we subscribe to myAmpacheService.connected? (Subscribing to it would allow reservation
    // of the vector size and also initialization of (max.) number of albums.)
    myAmpacheService.readyAlbums += bind(&AlbumRepository::onReadyAlbums, this, _1);
    myAmpacheService.readyAlbumArts += bind(&AlbumRepository::onReadyArts, this, _1);
}



/**
 * @warning Class does not work correctly if this method is called multiple times for the same data.
 */
bool AlbumRepository::load(int offset, int limit) {
    // SMELL: Use exceptions?
    if (myLoadOffset != -1) {
        return false;
    }
    myLoadOffset = offset;
    myAmpacheService.requestAlbums(offset, limit);
    return true;
}



Album& AlbumRepository::get(int filteredOffset) const {
    AlbumData& albumData = myAlbumDataReferences[filteredOffset];
    return albumData.getAlbum();
}



Album& AlbumRepository::getById(const string& id) const {
    return getAlbumDataById(id).getAlbum();
}



AlbumData& AlbumRepository::getAlbumDataById(const string& id) const {
    auto albumsDataIter = find_if(myAlbumsData.begin(), myAlbumsData.end(),
        [&id](const unique_ptr<AlbumData>& ad) {return ad->getId() == id;});
    return **albumsDataIter;
}



bool AlbumRepository::loadArts(int offset, int limit) {
    if (myArtsLoadOffset != -1) {
        return false;
    }

    myArtsLoadOffset = offset;
    vector<string> urls;
    for (auto idx = offset; idx < offset + limit; idx++) {
        urls.push_back(myAlbumsData[idx]->getArtUrl());
    }

    myAmpacheService.requestAlbumArts(urls);
    return true;
}



bool AlbumRepository::isLoaded(int filteredOffset, int limit) const {
    uint end = filteredOffset + limit;
    return (myAlbumDataReferences.size() >= end) && all_of(myAlbumDataReferences.begin() + filteredOffset,
        myAlbumDataReferences.begin() + filteredOffset + limit, [](const AlbumData& ad) {return &ad != nullptr;});
}



int AlbumRepository::maxCount() {
    if (myCachedMaxCount == -1) {
        myCachedMaxCount = computeMaxCount();
    }
    return myCachedMaxCount;
}



/**
 * @warning May be called no sooner than after the repository is fully loaded.
 */
void AlbumRepository::setArtistFilter(vector<reference_wrapper<const Artist>> artists) {
    unsetFilter();
    myIsFilterSet = true;
    unordered_set<reference_wrapper<AlbumData>, hash<AlbumData>> filteredUniqueAlbumData;
    for (auto artist: artists) {
        auto artistIndex = (*myArtistIndex)[artist];
        filteredUniqueAlbumData.insert(artistIndex.begin(), artistIndex.end());
    }
    myAlbumDataReferences.swap(myStoredAlbumDataReferences);
    for (auto albumData: filteredUniqueAlbumData) {
        myAlbumDataReferences.push_back(albumData);
    }

    myCachedMaxCount = -1;

    bool b = false;
    filterChanged(b);
}



/**
 * @warning May be called no sooner than after the repository is fully loaded.
 */
void AlbumRepository::setNameFilter(const string& namePattern) {
    unsetFilter();
    myIsFilterSet = true;
    vector<reference_wrapper<AlbumData>> filteredAlbumData;
    for (auto& albumData: myAlbumsData) {
        auto name = albumData->getAlbum().getName();
        if (search(name.begin(), name.end(), namePattern.begin(), namePattern.end(),
            [](char c1, char c2) {return toupper(c1) == toupper(c2); }) != name.end()) {

            filteredAlbumData.push_back(*albumData);
        }
    }
    myAlbumDataReferences.swap(myStoredAlbumDataReferences);
    myAlbumDataReferences.swap(filteredAlbumData);

    myCachedMaxCount = -1;

    bool b = false;
    filterChanged(b);
}



void AlbumRepository::unsetFilter() {
    if (!myIsFilterSet) {
        return;
    }
    myAlbumDataReferences.clear();
    myAlbumDataReferences.swap(myStoredAlbumDataReferences);
    myIsFilterSet = false;

    myCachedMaxCount = -1;

    bool b = false;
    filterChanged(b);
}



void AlbumRepository::setArtistIndex(unique_ptr<ArtistAlbumVectorIndex> artistIndex) {
    myArtistIndex = move(artistIndex);
}



void AlbumRepository::onReadyAlbums(vector<unique_ptr<AlbumData>>& albumsData) {
    uint offset = myLoadOffset;
    auto end = offset + albumsData.size();
    if (end > myAlbumsData.size()) {
        myAlbumsData.resize(end);

        // resize references container
        for (auto idx = myAlbumDataReferences.size(); idx < end; idx++) {
            myAlbumDataReferences.push_back(*myAlbumsData[idx]);
        }
    }

    for (auto& albumData: albumsData) {
        if (albumData->hasArtist()) {
            auto& artist = myArtistRepository.getById(albumData->getArtistId());
            albumData->getAlbum().setArtist(artist);
        }

        myAlbumDataReferences[offset] = *albumData;
        myAlbumsData[offset] = move(albumData);
        offset++;
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, albumsData.size()};
    myLoadOffset = -1;
    myCachedMaxCount = -1;

    loaded(offsetAndLimit);

    myLoadProgress += albumsData.size();
    if (myLoadProgress >= myAmpacheService.numberOfAlbums()) {
        bool b = false;
        fullyLoaded(b);
    }
}



void AlbumRepository::onReadyArts(std::map<std::string, QPixmap>& arts) {
    uint offset = myArtsLoadOffset;

    for (auto urlAndArt: arts) {
        auto albumDataIter = find_if(myAlbumsData.begin() + offset, myAlbumsData.begin() + offset + arts.size(),
            [&urlAndArt](unique_ptr<AlbumData>& ad) {return ad->getArtUrl() == urlAndArt.first;});
        (*albumDataIter)->getAlbum().setArt(unique_ptr<QPixmap>{new QPixmap{urlAndArt.second}});
    }

    auto offsetAndLimit = pair<int, int>{myArtsLoadOffset, arts.size()};
    myArtsLoadOffset = -1;
    artsLoaded(offsetAndLimit);
}



int AlbumRepository::computeMaxCount() const {
    if (myIsFilterSet) {
        return myAlbumDataReferences.size();
    }
    return myAmpacheService.numberOfAlbums();
}

}
