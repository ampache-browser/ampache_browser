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
#include "data/cache/cache.h"
#include "album_data.h"
#include "data/artist_repository.h"
#include "data/album_repository.h"
#include "data/indices.h"

using namespace std;
using namespace placeholders;
using namespace domain;



namespace data {

AlbumRepository::AlbumRepository(AmpacheService& ampacheService, Cache& cache, ArtistRepository& artistRepository,
    Indices& indices):
myAmpacheService(ampacheService),
myCache(cache),
myArtistRepository(artistRepository),
myIndices(indices) {
    // SMELL: Should we subscribe to myAmpacheService.connected? (Subscribing to it would allow reservation
    // of the vector size and also initialization of (max.) number of albums.)
    myAmpacheService.readyAlbums += bind(&AlbumRepository::onReadyAlbums, this, _1);
    myAmpacheService.readyAlbumArts += bind(&AlbumRepository::onReadyArts, this, _1);
    myIndices.changed += bind(&AlbumRepository::onIndexChanged, this, _1);
}



/**
 * @warning Class does not work correctly if this method is called multiple times for the same data.
 */
bool AlbumRepository::load(int offset, int limit) {
    // SMELL: Use exceptions?
    if (myLoadOffset != -1) {
        return false;
    }

    if (myCache.getLastUpdate() > myAmpacheService.getLastUpdate()) {
        myCachedLoad = true;

        // SMELL: The condition below is to ignore subsequent requests from model which are not needed since the
        // repository was fully loaded at the first load() call.  This is inconsistent from non-cached loads and it
        // would be better if it was handled by the caller.
        if (myLoadProgress == 0) {
            loadFromCache();
        }

    } else {
        myCachedLoad = false;
        myLoadOffset = offset;
        myAmpacheService.requestAlbums(offset, limit);
    }
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



/**
 * @warning Albums for requested arts have to be loaded (via load() method) prior to calling this method.
 */
bool AlbumRepository::loadArts(int filteredOffset, int limit) {
    if (myArtsLoadOffset != -1) {
        return false;
    }

    if (myCachedLoad) {
        vector<string> artIds;
        for (auto idx = filteredOffset; idx < filteredOffset + limit; idx++) {
            AlbumData& albummData = myAlbumDataReferences[idx];
            auto id = albummData.getId();
            artIds.push_back(id);
        }
        auto arts = myCache.loadAlbumArts(artIds);
        // TODO: Optimize so that it loops only over the same albums as the loop above (the loop above will store
        // also album data).
        for (auto idAndArt: arts) {
            auto& albumData = getAlbumDataById(idAndArt.first);
            albumData.getAlbum().setArt(unique_ptr<QPixmap>{new QPixmap{idAndArt.second}});
        }

        auto offsetAndLimit = pair<int, int>{filteredOffset, limit};
        myArtsLoadOffset = -1;
        artsLoaded(offsetAndLimit);
    } else {
        myArtsLoadOffset = filteredOffset;
        vector<string> urls;
        for (auto idx = filteredOffset; idx < filteredOffset + limit; idx++) {
            AlbumData& albumData = myAlbumDataReferences[idx];
            urls.push_back(albumData.getArtUrl());
        }

        myAmpacheService.requestAlbumArts(urls);
    }
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
    myArtistFilter = artists;
    myAlbumDataReferences.swap(myStoredAlbumDataReferences);
    updateArtistFilter();
}



/**
 * @warning May be called no sooner than after the repository is fully loaded.
 */
void AlbumRepository::setNameFilter(const string& namePattern) {
    unsetFilter();
    myNameFilter = namePattern;
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
    if (!isFiltered()) {
        return;
    }
    myAlbumDataReferences.clear();
    myAlbumDataReferences.swap(myStoredAlbumDataReferences);
    myArtistFilter.clear();
    myNameFilter = "";

    myCachedMaxCount = -1;

    bool b = false;
    filterChanged(b);
}



bool AlbumRepository::isFiltered() const {
    return !myArtistFilter.empty() || !myNameFilter.empty();
}



void AlbumRepository::onReadyAlbums(vector<unique_ptr<AlbumData>>& albumsData) {
    if (isFiltered()) {
        myAlbumDataReferences.swap(myStoredAlbumDataReferences);
    }

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

    if (isFiltered()) {
        myAlbumDataReferences.swap(myStoredAlbumDataReferences);
        if (!myArtistFilter.empty()) {
            updateArtistFilter();
        }
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, albumsData.size()};
    myLoadOffset = -1;
    myCachedMaxCount = -1;

    loaded(offsetAndLimit);

    myLoadProgress += albumsData.size();
    if (myLoadProgress >= myAmpacheService.numberOfAlbums()) {
        myCache.saveAlbumsData(myAlbumsData);

        bool b = false;
        fullyLoaded(b);
    }
}



void AlbumRepository::onReadyArts(std::map<std::string, QPixmap>& arts) {
    // there might be some change during loading (e.g. filter was changed) so ignore the result
    if (myArtsLoadOffset == -1) {
        auto offsetAndLimit = pair<int, int>{0, 0};
        artsLoaded(offsetAndLimit);
        return;
    }

    map<string, QPixmap> idAndArts;
    for (auto urlAndArt: arts) {
        auto albumDataIter = find_if(myAlbumDataReferences.begin() + myArtsLoadOffset,
            myAlbumDataReferences.begin() + myArtsLoadOffset + arts.size(),
            [&urlAndArt](AlbumData& ad) {return ad.getArtUrl() == urlAndArt.first;});
        AlbumData& albumData = *albumDataIter;
        auto& album = albumData.getAlbum();

        album.setArt(unique_ptr<QPixmap>{new QPixmap{urlAndArt.second}});

        idAndArts.emplace(album.getId(), urlAndArt.second);
    }

    myCache.updateAlbumArts(idAndArts);

    auto offsetAndLimit = pair<int, int>{myArtsLoadOffset, arts.size()};
    myArtsLoadOffset = -1;
    artsLoaded(offsetAndLimit);
}



void AlbumRepository::onIndexChanged(bool&) {
    if (!myArtistFilter.empty()) {
        updateArtistFilter();
    }
}



void AlbumRepository::updateArtistFilter() {
    myAlbumDataReferences.clear();
    unordered_set<reference_wrapper<AlbumData>, hash<AlbumData>> filteredUniqueAlbumData;

    // TODO: Include also albums that have matching album artist.
    for (auto artist: myArtistFilter) {
        auto artistIndex = myIndices.getArtistAlbum()[artist];
        filteredUniqueAlbumData.insert(artistIndex.begin(), artistIndex.end());
    }
    for (auto albumData: filteredUniqueAlbumData) {
        myAlbumDataReferences.push_back(albumData);
    }

    myArtsLoadOffset = -1;
    myCachedMaxCount = -1;

    bool b = false;
    filterChanged(b);
}



void AlbumRepository::loadFromCache() {
    myAlbumsData = myCache.loadAlbumsData();

    for (auto& albumData: myAlbumsData) {
        if (albumData->hasArtist()) {
            auto& artist = myArtistRepository.getById(albumData->getArtistId());
            albumData->getAlbum().setArtist(artist);
        }

        myAlbumDataReferences.push_back(*albumData);
    }

    myLoadOffset = -1;
    myCachedMaxCount = -1;

    auto offsetAndLimit = pair<int, int>{0, myAlbumsData.size()};
    loaded(offsetAndLimit);

    myLoadProgress += myAlbumsData.size();
    bool b = false;
    fullyLoaded(b);
}



int AlbumRepository::computeMaxCount() const {
    if (isFiltered() && myLoadProgress != 0) {
        return myAlbumDataReferences.size();
    }
    return myAmpacheService.numberOfAlbums();
}

}
