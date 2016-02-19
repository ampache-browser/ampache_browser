// album_repository.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <iterator>
#include <unordered_set>

#include "infrastructure/event/delegate.h"
#include "domain/artist.h"
#include "data/providers/ampache_service.h"
#include "data/providers/cache.h"
#include "data_objects/album_data.h"
#include "data/indices.h"
#include "data/filters/filter.h"
#include "data/artist_repository.h"
#include "data/album_repository.h"


using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

AlbumRepository::AlbumRepository(AmpacheService& ampacheService, Cache& cache,
    const ArtistRepository& artistRepository, Indices& indices):
myAmpacheService(ampacheService),
myCache(cache),
myArtistRepository(artistRepository),
myIndices{indices} {
    myUnfilteredFilter->setSourceData(myAlbumsData);
    myUnfilteredFilter->changed += DELEGATE0(&AlbumRepository::onFilterChanged);
    myFilter = myUnfilteredFilter;

    // SMELL: Should we subscribe to myAmpacheService.connected? (Subscribing to it would allow reservation
    // of the vector size and also initialization of (max.) number of albums.)
    myAmpacheService.readyAlbums += DELEGATE1(&AlbumRepository::onReadyAlbums, vector<unique_ptr<AlbumData>>);
    myAmpacheService.readyAlbumArts += DELEGATE1(&AlbumRepository::onAmpacheReadyArts, map<string, QPixmap>);
    myCache.readyAlbumArts += DELEGATE1(&AlbumRepository::onCacheReadyArts, map<string, QPixmap>);
}



AlbumRepository::~AlbumRepository() {
    myCache.readyAlbumArts -= DELEGATE1(&AlbumRepository::onCacheReadyArts, map<string, QPixmap>);
    myAmpacheService.readyAlbumArts -= DELEGATE1(&AlbumRepository::onAmpacheReadyArts, map<string, QPixmap>);
    myAmpacheService.readyAlbums -= DELEGATE1(&AlbumRepository::onReadyAlbums, vector<unique_ptr<AlbumData>>);
    myUnfilteredFilter->changed -= DELEGATE0(&AlbumRepository::onFilterChanged);
    if (isFiltered()) {
        myFilter->changed -= DELEGATE0(&AlbumRepository::onFilterChanged);
    }
}



/**
 * @warning Class does not work correctly if this method is called multiple times for the same data.
 */
bool AlbumRepository::load(int offset, int limit) {
    // SMELL: Use exceptions?
    if (myLoadOffset != -1) {
        return false;
    }

    if (!myAmpacheService.getIsConnected() || (myCache.getLastUpdate() > myAmpacheService.getLastUpdate())) {
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
    AlbumData& albumData = myFilter->getFilteredData()[filteredOffset];
    return albumData.getAlbum();
}



Album& AlbumRepository::getById(const string& id) const {
    return getAlbumDataById(id).getAlbum();
}



AlbumData& AlbumRepository::getAlbumDataById(const string& id) const {
    // SMELL: ad can be nullptr.
    // SMELL: in case album data is not found, return null?
    auto albumsDataIter = find_if(myAlbumsData.begin(), myAlbumsData.end(),
        [&id](const unique_ptr<AlbumData>& ad) {return ad->getId() == id;});
    return **albumsDataIter;
}



/**
 * @warning Albums for requested arts have to be loaded (via load() method) prior to calling this method.
 */
bool AlbumRepository::loadArts(int filteredOffset, int count) {
    if (myArtsLoadOffset != -1) {
        return false;
    }

    myArtsLoadOffset = filteredOffset;
    myArtsLoadCount = count;
    vector<string> albumIds;
    for (auto idx = filteredOffset; idx < filteredOffset + count; idx++) {
        AlbumData& albumData = myFilter->getFilteredData()[idx];
        albumIds.push_back(albumData.getId());
    }
    if (myCachedLoad) {
        myCache.requestAlbumArts(albumIds);
    } else {
        myAmpacheService.requestAlbumArts(albumIds);
    }
    return true;
}



bool AlbumRepository::isLoaded(int filteredOffset, int count) const {
    uint end = filteredOffset + count;
    auto filteredAlbumsData = myFilter->getFilteredData();
    return (filteredAlbumsData.size() >= end) && all_of(filteredAlbumsData.begin() + filteredOffset,
        filteredAlbumsData.begin() + filteredOffset + count, [](const AlbumData& ad) {return &ad != nullptr;});
}



int AlbumRepository::maxCount() {
    if (myCachedMaxCount == -1) {
        myCachedMaxCount = computeMaxCount();
    }
    return myCachedMaxCount;
}



void AlbumRepository::setFilter(unique_ptr<Filter<AlbumData>> filter) {
    myIsFilterSet = true;

    myFilter->changed -= DELEGATE0(&AlbumRepository::onFilterChanged);

    filter->setSourceData(myAlbumsData);
    filter->changed += DELEGATE0(&AlbumRepository::onFilterChanged);
    myFilter = move(filter);
    myFilter->apply();
}



void AlbumRepository::unsetFilter() {
    if (!isFiltered()) {
        return;
    }
    myIsFilterSet = false;

    myFilter->changed -= DELEGATE0(&AlbumRepository::onFilterChanged);

    myUnfilteredFilter->changed += DELEGATE0(&AlbumRepository::onFilterChanged);
    myFilter = myUnfilteredFilter;
    myCachedMaxCount = -1;

    filterChanged();
}



bool AlbumRepository::isFiltered() const {
    return myIsFilterSet;
}



void AlbumRepository::onReadyAlbums(vector<unique_ptr<AlbumData>>& albumsData) {
    uint offset = myLoadOffset;
    auto end = offset + albumsData.size();
    if (end > myAlbumsData.size()) {
        myAlbumsData.resize(end);
    }

    for (auto& albumData: albumsData) {
        if (albumData->hasArtist()) {
            auto& artist = myArtistRepository.getById(albumData->getArtistId());
            albumData->getAlbum().setArtist(artist);

            updateIndices(*albumData);
        }

        myAlbumsData[offset] = move(albumData);
        offset++;
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, albumsData.size()};
    myUnfilteredFilter->processUpdatedSourceData(myLoadOffset, albumsData.size());
    myFilter->apply();
    myLoadOffset = -1;
    myLoadProgress += albumsData.size();

    bool isFullyLoaded = myLoadProgress >= myAmpacheService.numberOfAlbums();
    if (isFullyLoaded) {
        myCache.saveAlbumsData(myAlbumsData);
    }

    // application can be terminated after loaded event therefore there should be no access to instance variables
    // after it is fired
    loaded(offsetAndLimit);
    if (isFullyLoaded) {
        fullyLoaded();
    }
}



void AlbumRepository::onAmpacheReadyArts(const map<string, QPixmap>& arts) {
    if (raiseEmptyIfResultNotValid()) {
        return;
    }

    map<string, QPixmap> loadedIdsAndArts;
    for (auto& idAndArt: arts) {
        Album* album = nullptr;
        if (!idAndArt.second.isNull()) {
            album = findFilteredAlbumById(idAndArt.first, myArtsLoadOffset, myArtsLoadCount);
        }
        if (album == nullptr) {
            continue;
        }

        album->setArt(unique_ptr<QPixmap>{new QPixmap{idAndArt.second}});
        loadedIdsAndArts.emplace(idAndArt);
    }

    myCache.updateAlbumArts(loadedIdsAndArts);

    // application can be terminated after artsLoaded event therefore there should be no access to instance variables
    // after it is fired
    auto offsetAndLimit = pair<int, int>{myArtsLoadOffset, myArtsLoadCount};
    myArtsLoadOffset = -1;
    myArtsLoadCount = -1;
    artsLoaded(offsetAndLimit);
}



void AlbumRepository::onCacheReadyArts(const map<string, QPixmap>& arts) {
    if (raiseEmptyIfResultNotValid()) {
        return;
    }

    vector<string> notLoadedArtIds;
    for (auto& idAndArt: arts) {
        Album* album = nullptr;
        if (!idAndArt.second.isNull()) {
            album = findFilteredAlbumById(idAndArt.first, myArtsLoadOffset, myArtsLoadCount);
        } else {
            notLoadedArtIds.push_back(idAndArt.first);
        }
        if (album == nullptr) {
            continue;
        }

        album->setArt(unique_ptr<QPixmap>{new QPixmap{idAndArt.second}});
    }

    myAmpacheService.requestAlbumArts(notLoadedArtIds);
}



void AlbumRepository::onFilterChanged() {
    myArtsLoadOffset = -1;
    myArtsLoadCount = -1;
    myCachedMaxCount = -1;

    // to the outside world there is no filter to change if not filtered
    if (isFiltered()) {
        filterChanged();
    }
}



void AlbumRepository::loadFromCache() {
    myAlbumsData = myCache.loadAlbumsData();

    for (auto& albumData: myAlbumsData) {
        if (albumData->hasArtist()) {
            auto& artist = myArtistRepository.getById(albumData->getArtistId());
            albumData->getAlbum().setArtist(artist);

            updateIndices(*albumData);
        }
    }

    myUnfilteredFilter->processUpdatedSourceData(0, myAlbumsData.size());
    myFilter->apply();
    myLoadOffset = -1;
    myLoadProgress += myAlbumsData.size();

    // application can be terminated after loaded event therefore there should be no access to instance variables
    // after it is fired
    auto offsetAndLimit = pair<int, int>{0, myAlbumsData.size()};
    loaded(offsetAndLimit);
    fullyLoaded();
}



void AlbumRepository::updateIndices(AlbumData& albumData) {
    myIndices.addAlbum(albumData.getAlbum());

    // Ampache (3.7.0) seems to ignore Album Artist info.  Only single-artist albums have Artist ID set.  Albums with
    // various artists does not (even if the Album Artist is set in the track's tags).  Therefore this code is
    // redundand; it is kept however in case Ampache is fixed.
    auto& artist = myArtistRepository.getById(albumData.getArtistId());
    myIndices.updateArtistAlbums(artist, albumData);
}



int AlbumRepository::computeMaxCount() const {
    if (isFiltered() && myLoadProgress != 0) {
        return myFilter->getFilteredData().size();
    }
    return myAmpacheService.getIsConnected() ? myAmpacheService.numberOfAlbums() : myCache.numberOfAlbums();
}



Album* AlbumRepository::findFilteredAlbumById(const string& id, int offset, int count) const {
    auto filteredAlbumsData = myFilter->getFilteredData();
    auto albumDataIter = filteredAlbumsData.begin() + offset + count;

    albumDataIter = find_if(filteredAlbumsData.begin() + offset,
        filteredAlbumsData.begin() + offset + count,
        [&id](AlbumData& ad) {return ad.getId() == id;});
    if (albumDataIter == filteredAlbumsData.begin() + offset + count) {
        return nullptr;
    }
    return &(*albumDataIter).get().getAlbum();
}



/**
 * @brief If myArtsLoadOffset is -1 raises that no arts are loaded.
 *
 * If myArtsLoadOffset is -1 it indicates that the result is invalid due to some change during arts loading
 * (e.g. filter was changed).
 *
 * @return true if result is invalid and the event was raised.
 */
bool AlbumRepository::raiseEmptyIfResultNotValid() const {
    // there might be some change during loading (e.g. filter was changed) so ignore the result
    if (myArtsLoadOffset == -1) {
        auto offsetAndLimit = pair<int, int>{0, 0};
        artsLoaded(offsetAndLimit);
        return true;
    }
    return false;
}

}
