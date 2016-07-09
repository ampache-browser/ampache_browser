// album_repository.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <string>
#include <vector>

#include "infrastructure/logging/logging.h"
#include "infrastructure/event/delegate.h"
#include "domain/artist.h"
#include "data/provider_type.h"
#include "data/providers/ampache.h"
#include "data/providers/cache.h"
#include "../data_objects/album_data.h"
#include "data/indices.h"
#include "data/repositories/artist_repository.h"
#include "data/repositories/album_repository.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;



namespace data {

AlbumRepository::AlbumRepository(Ampache& ampache, Cache& cache, Indices& indices,
    const ArtistRepository* const artistRepository): Repository<AlbumData, Album>(ampache, cache, indices),
myArtistRepository(artistRepository) {
    myAmpache.readyAlbumArts += DELEGATE1(&AlbumRepository::onAmpacheReadyArts, map<string, QPixmap>);
    myCache.readyAlbumArts += DELEGATE1(&AlbumRepository::onCacheReadyArts, map<string, QPixmap>);
}



AlbumRepository::~AlbumRepository() {
    myCache.readyAlbumArts -= DELEGATE1(&AlbumRepository::onCacheReadyArts, map<string, QPixmap>);
    myAmpache.readyAlbumArts -= DELEGATE1(&AlbumRepository::onAmpacheReadyArts, map<string, QPixmap>);
}



void AlbumRepository::setProviderType(ProviderType providerType) {
    Repository<AlbumData, Album>::setProviderType(providerType);

    if (myProviderType != providerType && maxCount() == 0) {
        auto error = false;
        artsFullyLoaded(error);
    }
}



Album& AlbumRepository::getUnfiltered(int offset) const {
    return getDomainObject(*(myData.at(offset)));
}



AlbumData* AlbumRepository::getAlbumDataById(const string& id) const {
    auto albumsDataIter = find_if(myData.begin(), myData.end(),
        [&id](const unique_ptr<AlbumData>& ad) {return ad != nullptr && ad->getId() == id;});
    return albumsDataIter != myData.end() ? albumsDataIter->get() : nullptr;
}



bool AlbumRepository::isLoadedUnfiltered(int offset, int count) const {
    uint end = offset + count;
    return (myData.size() >= end) && all_of(myData.begin() + offset,
        myData.begin() + offset + count, [](const std::unique_ptr<AlbumData>& d) {return d != nullptr;});
}



/**
 * @warning Albums for requested arts have to be loaded (via load() method) prior to calling this method.
 */
bool AlbumRepository::loadArts(int filteredOffset, int count) {
    if (myArtsLoadOffset != -1 || myArtsLoadOffsetUnfiltered != -1 || !myLoadingEnabled) {
        return false;
    }

    LOG_DBG("Load arts from filtered offset %d, count %d.", filteredOffset, count);
    myArtsLoadOffset = filteredOffset;
    myArtsLoadCount = count;
    if (myProviderType == ProviderType::Ampache) {
        map<string, string> albumIdsAndUrls;
        for (auto idx = filteredOffset; idx < filteredOffset + count; idx++) {
            AlbumData* albumData = myFilter->getFilteredData()[idx];
            albumIdsAndUrls[albumData->getId()] = albumData->getArtUrl();
        }
        myAmpache.requestAlbumArts(albumIdsAndUrls);
    } else if (myProviderType == ProviderType::Cache) {
        vector<string> albumIds;
        for (auto idx = filteredOffset; idx < filteredOffset + count; idx++) {
            AlbumData* albumData = myFilter->getFilteredData()[idx];
            albumIds.push_back(albumData->getId());
        }
        myCache.requestAlbumArts(albumIds);
    }
    return true;
}



/**
 * @warning Albums for requested arts have to be loaded (via load() method) prior to calling this method.
 */
bool AlbumRepository::loadArtsUnfiltered(int offset, int count) {
    if (myArtsLoadOffset != -1 || myArtsLoadOffsetUnfiltered != -1 || !myLoadingEnabled) {
        return false;
    }

    LOG_DBG("Load arts from offset %d, count %d.", offset, count);
    myArtsLoadOffsetUnfiltered = offset;
    myArtsLoadCount = count;
    if (myProviderType == ProviderType::Ampache) {
        map<string, string> albumIdsAndUrls;
        for (auto idx = offset; idx < offset + count; idx++) {
            auto& albumData = myData[idx];
            albumIdsAndUrls[albumData->getId()] = albumData->getArtUrl();
        }
        myAmpache.requestAlbumArts(albumIdsAndUrls);
    } else if (myProviderType == ProviderType::Cache) {
        vector<string> albumIds;
        for (auto idx = offset; idx < offset + count; idx++) {
            auto& albumData = myData[idx];
            albumIds.push_back(albumData->getId());
        }
        myCache.requestAlbumArts(albumIds);
    }
    return true;
}



int AlbumRepository::maxCount() const {
    if (myProviderType == ProviderType::Ampache) {
        return myAmpache.numberOfAlbums();
    };
    if (myProviderType == ProviderType::Cache) {
        return myCache.numberOfAlbums();
    };
    return 0;
}



void AlbumRepository::disableLoading() {
    Repository<AlbumData, Album>::disableLoading();

    if (myArtsLoadOffset == -1 && myArtsLoadOffsetUnfiltered == -1) {
        artsLoadingDisabled();
    }
}



void AlbumRepository::requestDataLoad(int offset, int limit) {
    myAmpache.requestAlbums(offset, limit);
}



Album& AlbumRepository::getDomainObject(const AlbumData& dataItem) const {
    return dataItem.getAlbum();
}



Event<pair<vector<unique_ptr<AlbumData>>, bool>>& AlbumRepository::getDataLoadRequestFinishedEvent() {
    return myAmpache.readyAlbums;
}



void AlbumRepository::loadDataFromCache() {
    myData = myCache.loadAlbumsData();
}



void AlbumRepository::saveDataToCache() {
    myCache.saveAlbumsData(myData);
}



void AlbumRepository::handleLoadedItem(const AlbumData& dataItem) const {
    if (dataItem.hasArtist()) {
        auto artist = myArtistRepository->getById(dataItem.getArtistId());
        if (artist != nullptr) {
            dataItem.getAlbum().setArtist(*artist);
        }
    }
}



void AlbumRepository::updateIndices(const vector<unique_ptr<AlbumData>>& data) {
    vector<reference_wrapper<Album>> albums;
    ArtistAlbumsIndex artistAlbums;
    for (auto& dataItem: data) {
        albums.push_back(dataItem->getAlbum());
        if (dataItem->hasArtist()) {

            // Ampache (3.7.0) seems to ignore Album Artist info.  Only single-artist albums have Artist ID set.  Albums
            // with various artists does not (even if the Album Artist is set in the track's tags).  Therefore
            // this code is redundand; it is kept however in case Ampache is fixed.
            auto artist = myArtistRepository->getById(dataItem->getArtistId());
            if (artist != nullptr) {
                artistAlbums[*artist].insert(*dataItem);
            }
        }
    }
    myIndices.addAlbums(albums);
    myIndices.updateArtistAlbums(artistAlbums);
}



void AlbumRepository::clear() {
    Repository<AlbumData, Album>::clear();

    myArtsLoadProgress = 0;
    myArtsLoadOffset = -1;
    myArtsLoadOffsetUnfiltered = -1;
}



void AlbumRepository::clearIndices() {
    myIndices.clearAlbums();
    myIndices.clearArtistsAlbums();
}



void AlbumRepository::handleFilterSetUnsetOrChanged() {
    Repository<AlbumData, Album>::handleFilterSetUnsetOrChanged();

    // SMELL: Not necessary if unfiltered filter has changed.
    myArtsLoadOffset = -1;
}



void AlbumRepository::onAmpacheReadyArts(const map<string, QPixmap>& arts) {
    LOG_DBG("Ready %d art entries from filtered offset %d; offset %d; requested count was %d.", arts.size(),
        myArtsLoadOffset, myArtsLoadOffsetUnfiltered, myArtsLoadCount);

    if (!myLoadingEnabled) {
        artsLoadingDisabled();
        return;
    }

    if (arts.size() == 0) {
        auto error = true;
        artsFullyLoaded(error);
        return;
    }

    auto loadedIdsAndArts = setArts(arts).first;

    myCache.updateAlbumArts(loadedIdsAndArts);
    myArtsLoadProgress += loadedIdsAndArts.size();
    LOG_DBG("Arts load progress: %d.", myArtsLoadProgress);

    fireArtsLoadedEvents();
}



void AlbumRepository::onCacheReadyArts(const map<string, QPixmap>& arts) {
    LOG_DBG("Ready %d art entries from filtered offset %d; offset %d; requested count was %d.", arts.size(),
        myArtsLoadOffset, myArtsLoadOffsetUnfiltered, myArtsLoadCount);

    auto loadedAndNotLoadedIds = setArts(arts);
    auto loadedIdsAndArts = loadedAndNotLoadedIds.first;
    auto notLoadedIdsAndUrls = loadedAndNotLoadedIds.second;

    // increase progress by number of 'loaded IDs' which are not in 'not loaded IDs'
    for (auto& loadedIdAndArt: loadedIdsAndArts) {
        auto notLoadedIdsIter = find_if(notLoadedIdsAndUrls.begin(), notLoadedIdsAndUrls.end(),
            [&loadedIdAndArt](const pair<string, string>& nliu) {return nliu.first == loadedIdAndArt.first;});
        if (notLoadedIdsIter == notLoadedIdsAndUrls.end()) {
            myArtsLoadProgress++;
        }
    }

    if (notLoadedIdsAndUrls.size() != 0) {
        myAmpache.requestAlbumArts(notLoadedIdsAndUrls);
    } else {
        fireArtsLoadedEvents();
    }
}



pair<map<string, QPixmap>, map<string, string>> AlbumRepository::setArts(const map<string, QPixmap>& arts) {
    map<string, QPixmap> loadedIdsAndArts;
    map<string, string> notLoadedArtIds;
    if (myArtsLoadOffset != -1) {
        for (auto& idAndArt: arts) {
            auto albumData = findAlbumDataById(idAndArt.first, myArtsLoadOffset, myArtsLoadCount);
            if (albumData != nullptr) {
                // set the art even if the loaded image is empty (isNull()), otherwise the server would be queried
                // again and again next time
                albumData->getAlbum().setArt(unique_ptr<QPixmap>{new QPixmap{idAndArt.second}});
                loadedIdsAndArts.emplace(idAndArt);
                if (idAndArt.second.isNull()) {
                    notLoadedArtIds[idAndArt.first] = albumData->getArtUrl();
                }
            }
        }
    } else if (myArtsLoadOffsetUnfiltered != -1) {
        for (auto& idAndArt: arts) {
            auto albumData = findAlbumDataByIdUnfiltered(idAndArt.first, myArtsLoadOffsetUnfiltered, myArtsLoadCount);
            if (albumData != nullptr) {
                // set the art even if the loaded image is empty (isNull()), otherwise the server would be queried
                // again and again next time
                albumData->getAlbum().setArt(unique_ptr<QPixmap>{new QPixmap{idAndArt.second}});
                loadedIdsAndArts.emplace(idAndArt);
                if (idAndArt.second.isNull()) {
                    notLoadedArtIds[idAndArt.first] = albumData->getArtUrl();
                }
            }
        }
    } else {
        for (auto& idAndArt: arts) {
            auto albumData = getAlbumDataById(idAndArt.first);
            // set the art even if the loaded image is empty (isNull()), otherwise the server would be queried
            // again and again next time
            // it should not happen that albumData == nullptr because arts are requested only for existing albums
            albumData->getAlbum().setArt(unique_ptr<QPixmap>{new QPixmap{idAndArt.second}});
            loadedIdsAndArts.emplace(idAndArt);
            if (idAndArt.second.isNull()) {
                notLoadedArtIds[idAndArt.first] = albumData->getArtUrl();
            }
        }
    }

    return make_pair(loadedIdsAndArts, notLoadedArtIds);
}



AlbumData* AlbumRepository::findAlbumDataById(const string& id, int filteredOffset, int count) const {
    auto filteredAlbumsData = myFilter->getFilteredData();
    auto albumDataIter = find_if(filteredAlbumsData.begin() + filteredOffset,
        filteredAlbumsData.begin() + filteredOffset + count,
        [&id](AlbumData* ad) {return ad->getId() == id;});
    if (albumDataIter == filteredAlbumsData.begin() + filteredOffset + count) {
        return nullptr;
    }
    return *albumDataIter;
}



AlbumData* AlbumRepository::findAlbumDataByIdUnfiltered(const string& id, int offset, int count) const {
    auto albumDataIter = find_if(myData.begin() + offset, myData.begin() + offset + count,
        [&id](const unique_ptr<AlbumData>& ad) {return ad != nullptr && ad->getId() == id;});
    if (albumDataIter == myData.begin() + offset + count) {
        return nullptr;
    }
    return albumDataIter->get();
}



void AlbumRepository::fireArtsLoadedEvents() {
    auto offset = myArtsLoadOffset != -1 ? myArtsLoadOffset : myArtsLoadOffsetUnfiltered;
    auto offsetAndCount = offset != -1 ? pair<int, int>{offset, myArtsLoadCount} : pair<int, int>{0, 0};
    myArtsLoadOffset = -1;
    myArtsLoadOffsetUnfiltered = -1;
    myArtsLoadCount = -1;
    artsLoaded(offsetAndCount);
    if (myArtsLoadProgress >= maxCount()) {
        auto error = false;
        artsFullyLoaded(error);
    }
}

}
