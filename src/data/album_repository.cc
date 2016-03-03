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
#include "data/provider_type.h"
#include "data/providers/ampache.h"
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

AlbumRepository::AlbumRepository(Ampache& ampache, Cache& cache, const ArtistRepository& artistRepository,
    Indices& indices):
myAmpache(ampache),
myCache(cache),
myArtistRepository(artistRepository),
myIndices{indices} {
    myUnfilteredFilter->setSourceData(myAlbumsData);
    myUnfilteredFilter->changed += DELEGATE0(&AlbumRepository::onFilterChanged);
    myFilter = myUnfilteredFilter;

    myAmpache.readyAlbums += DELEGATE1(&AlbumRepository::onReadyAlbums, vector<unique_ptr<AlbumData>>);
    myAmpache.readyAlbumArts += DELEGATE1(&AlbumRepository::onAmpacheReadyArts, map<string, QPixmap>);
    myCache.readyAlbumArts += DELEGATE1(&AlbumRepository::onCacheReadyArts, map<string, QPixmap>);
}



AlbumRepository::~AlbumRepository() {
    myCache.readyAlbumArts -= DELEGATE1(&AlbumRepository::onCacheReadyArts, map<string, QPixmap>);
    myAmpache.readyAlbumArts -= DELEGATE1(&AlbumRepository::onAmpacheReadyArts, map<string, QPixmap>);
    myAmpache.readyAlbums -= DELEGATE1(&AlbumRepository::onReadyAlbums, vector<unique_ptr<AlbumData>>);
    myUnfilteredFilter->changed -= DELEGATE0(&AlbumRepository::onFilterChanged);
    if (isFiltered()) {
        myFilter->changed -= DELEGATE0(&AlbumRepository::onFilterChanged);
    }
}



void AlbumRepository::setProviderType(ProviderType providerType) {
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
bool AlbumRepository::load(int offset, int limit) {
    // SMELL: Use exceptions?
    if (myLoadOffset != -1 || !myLoadingEnabled) {
        return false;
    }

    if (myProviderType == ProviderType::Ampache) {
        myLoadOffset = offset;
        myAmpache.requestAlbums(offset, limit);
    } else if (myProviderType == ProviderType::Cache) {

        // SMELL: The condition below is to ignore subsequent requests from model which are not needed since the
        // repository was fully loaded at the first load() call.  This is inconsistent from non-cached loads and it
        // would be better if it was handled by the caller.
        if (myAlbumsLoadProgress == 0) {
            loadFromCache();
        }
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
    if (myArtsLoadOffset != -1 || !myLoadingEnabled) {
        return false;
    }

    myArtsLoadOffset = filteredOffset;
    myArtsLoadCount = count;
    vector<string> albumIds;
    for (auto idx = filteredOffset; idx < filteredOffset + count; idx++) {
        AlbumData& albumData = myFilter->getFilteredData()[idx];
        albumIds.push_back(albumData.getId());
    }
    if (myProviderType == ProviderType::Ampache) {
        myAmpache.requestAlbumArts(albumIds);
    } else if (myProviderType == ProviderType::Cache) {
        myCache.requestAlbumArts(albumIds);
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



void AlbumRepository::disableLoading() {
    myLoadingEnabled = false;
    myCachedMaxCount = -1;
    loadingDisabled();

    auto error = false;
    if (myLoadOffset == -1) {
        fullyLoaded(error);
    }
    if (myArtsLoadOffset == -1) {
        artsFullyLoaded(error);
    }
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

    if (albumsData.size() == 0) {
        error = true;
        fullyLoaded(error);
        return;
    }

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
    myAlbumsLoadProgress += albumsData.size();

    bool isFullyLoaded = myAlbumsLoadProgress >= myAmpache.numberOfAlbums();
    if (isFullyLoaded) {
        myCache.saveAlbumsData(myAlbumsData);
    }

    // application can be terminated after loaded event therefore there should be no access to instance variables
    // after it is fired
    loaded(offsetAndLimit);
    if (isFullyLoaded || !myLoadingEnabled) {
        fullyLoaded(error);
    }
}



void AlbumRepository::onAmpacheReadyArts(const map<string, QPixmap>& arts) {
    if (raiseEmptyIfResultNotValid()) {
        return;
    }

    map<string, QPixmap> loadedIdsAndArts;
    for (auto& idAndArt: arts) {
        auto album = findFilteredAlbumById(idAndArt.first, myArtsLoadOffset, myArtsLoadCount);
        if (album == nullptr) {
            continue;
        }

        album->setArt(unique_ptr<QPixmap>{new QPixmap{idAndArt.second}});
        loadedIdsAndArts.emplace(idAndArt);
    }

    myCache.updateAlbumArts(loadedIdsAndArts);
    myArtsLoadProgress += loadedIdsAndArts.size();

    // application can be terminated after artsLoaded event therefore there should be no access to instance variables
    // after it is fired
    auto offsetAndLimit = pair<int, int>{myArtsLoadOffset, myArtsLoadCount};
    myArtsLoadOffset = -1;
    myArtsLoadCount = -1;
    artsLoaded(offsetAndLimit);
    if ((myArtsLoadProgress >= computeUnfilteredMaxCount()) || !myLoadingEnabled) {
        auto error = false;
        artsFullyLoaded(error);
    }
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
        myArtsLoadProgress++;
    }

    myAmpache.requestAlbumArts(notLoadedArtIds);
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



void AlbumRepository::clear() {
    myAlbumsData.clear();
    myAlbumsLoadProgress = 0;
    myArtsLoadProgress = 0;
    myLoadOffset = -1;
    myCachedMaxCount = -1;

    myUnfilteredFilter->processUpdatedSourceData(-1, 0);
    myFilter->apply();
    myIndices.clearAlbums();
    myIndices.clearArtistsAlbums();
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
    myAlbumsLoadProgress += myAlbumsData.size();

    // application can be terminated after loaded event therefore there should be no access to instance variables
    // after it is fired
    auto offsetAndLimit = pair<int, int>{0, myAlbumsData.size()};
    loaded(offsetAndLimit);
    bool error = false;
    fullyLoaded(error);

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
    if (!myLoadingEnabled || (isFiltered() && myAlbumsLoadProgress != 0)) {
        return myFilter->getFilteredData().size();
    }
    return computeUnfilteredMaxCount();
}



int AlbumRepository::computeUnfilteredMaxCount() const {
    if (myProviderType == ProviderType::Ampache) {
        return myAmpache.numberOfAlbums();
    };
    if (myProviderType == ProviderType::Cache) {
        return myCache.numberOfAlbums();
    };
    return 0;
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
        // TODO: The {0, 0} result is not handled in model.  Check whether dataChanged(0, 0) does not trigger any
        // refresh.  If it triggers refresh then add if (zero result)... to event handler to avoid it.
        auto offsetAndLimit = pair<int, int>{0, 0};
        artsLoaded(offsetAndLimit);
        return true;
    }
    return false;
}

}
