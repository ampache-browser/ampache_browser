// album_repository.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <string>
#include <vector>

#include <libaudcore/runtime.h>

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



AlbumData& AlbumRepository::getAlbumDataById(const string& id) const {
    // SMELL: ad can be nullptr.
    // SMELL: in case album data is not found, return null?
    auto albumsDataIter = find_if(myData.begin(), myData.end(),
        [&id](const unique_ptr<AlbumData>& ad) {return ad->getId() == id;});
    return **albumsDataIter;
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

    AUDDBG("Load arts from filtered offset %d, count %d.\n", filteredOffset, count);
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



/**
 * @warning Albums for requested arts have to be loaded (via load() method) prior to calling this method.
 */
bool AlbumRepository::loadArtsUnfiltered(int offset, int count) {
    if (myArtsLoadOffset != -1 || myArtsLoadOffsetUnfiltered != -1 || !myLoadingEnabled) {
        return false;
    }

    AUDDBG("Load arts from offset %d, count %d.\n", offset, count);
    myArtsLoadOffsetUnfiltered = offset;
    myArtsLoadCount = count;
    vector<string> albumIds;
    for (auto idx = offset; idx < offset + count; idx++) {
        auto& albumData = myData[idx];
        albumIds.push_back(albumData->getId());
    }
    if (myProviderType == ProviderType::Ampache) {
        myAmpache.requestAlbumArts(albumIds);
    } else if (myProviderType == ProviderType::Cache) {
        myCache.requestAlbumArts(albumIds);
    }
    return true;
}



void AlbumRepository::disableLoading() {
    Repository<AlbumData, Album>::disableLoading();

    if (myArtsLoadOffset == -1 && myArtsLoadOffsetUnfiltered == -1) {
        artsLoadingDisabled();
    }
}



void AlbumRepository::onFilterChanged() {
    Repository<AlbumData, Album>::onFilterChanged();

    myArtsLoadOffset = -1;
}



void AlbumRepository::requestDataLoad(int offset, int limit) {
    myAmpache.requestAlbums(offset, limit);
}



Album& AlbumRepository::getDomainObject(const AlbumData& dataItem) const {
    return dataItem.getAlbum();
}



Event<vector<unique_ptr<AlbumData>>>& AlbumRepository::getDataLoadRequestFinishedEvent() {
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
        auto& artist = myArtistRepository->getById(dataItem.getArtistId());
        dataItem.getAlbum().setArtist(artist);
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
            auto& artist = myArtistRepository->getById(dataItem->getArtistId());
            artistAlbums[artist].insert(*dataItem);
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



int AlbumRepository::getMaxDataSize() const {
    if (myProviderType == ProviderType::Ampache) {
        return myAmpache.numberOfAlbums();
    };
    if (myProviderType == ProviderType::Cache) {
        return myCache.numberOfAlbums();
    };
    return 0;
}



void AlbumRepository::onAmpacheReadyArts(const map<string, QPixmap>& arts) {
    AUDDBG("Ready %d art entries from filtered offset %d; offset %d; requested count was %d.\n", arts.size(),
        myArtsLoadOffset, myArtsLoadOffsetUnfiltered, myArtsLoadCount);
    if (!myLoadingEnabled)
    {
        artsLoadingDisabled();
        return;
    }

    auto loadedIdsAndArts = setArts(arts).first;

    myCache.updateAlbumArts(loadedIdsAndArts);
    myArtsLoadProgress += loadedIdsAndArts.size();
    AUDDBG("Arts load progress: %d.\n", myArtsLoadProgress);

    auto offset = myArtsLoadOffset != -1 ? myArtsLoadOffset : myArtsLoadOffsetUnfiltered;
    auto offsetAndCount = offset != -1 ? pair<int, int>{offset, myArtsLoadCount} : pair<int, int>{0, 0};
    myArtsLoadOffset = -1;
    myArtsLoadOffsetUnfiltered = -1;
    myArtsLoadCount = -1;
    artsLoaded(offsetAndCount);
    if (myArtsLoadProgress >= getMaxDataSize()) {
        auto error = false;
        artsFullyLoaded(error);
    }
}



void AlbumRepository::onCacheReadyArts(const map<string, QPixmap>& arts) {
    AUDDBG("Ready %d art entries from filtered offset %d; offset %d; requested count was %d.\n", arts.size(),
        myArtsLoadOffset, myArtsLoadOffsetUnfiltered, myArtsLoadCount);

    auto loadedAndNotLoadedIds = setArts(arts);
    auto loadedIdsAndArts = loadedAndNotLoadedIds.first;
    auto notLoadedArtIds = loadedAndNotLoadedIds.second;

    // increase progress by number of loaded IDs which are not in not loaded IDs
    for (auto& loadedIdAndArt: loadedIdsAndArts) {
        auto notLoadedIdsIter = find_if(notLoadedArtIds.begin(), notLoadedArtIds.end(),
            [&loadedIdAndArt](const string& nli) {return nli == loadedIdAndArt.first;});
        if (notLoadedIdsIter == notLoadedArtIds.end()) {
            myArtsLoadProgress++;
        }
    }

    myAmpache.requestAlbumArts(notLoadedArtIds);
}



pair<map<string, QPixmap>, vector<string>> AlbumRepository::setArts(const map<string, QPixmap>& arts) {
    map<string, QPixmap> loadedIdsAndArts;
    vector<string> notLoadedArtIds;
    if (myArtsLoadOffset != -1) {
        for (auto& idAndArt: arts) {
            if (idAndArt.second.isNull()) {
                notLoadedArtIds.push_back(idAndArt.first);
                // go on and set the art, otherwise the server would be queried again and again next time
            }
            auto album = findById(idAndArt.first, myArtsLoadOffset, myArtsLoadCount);
            if (album != nullptr) {
                album->setArt(unique_ptr<QPixmap>{new QPixmap{idAndArt.second}});
                loadedIdsAndArts.emplace(idAndArt);
            }
        }
    } else if (myArtsLoadOffsetUnfiltered != -1) {
        for (auto& idAndArt: arts) {
            if (idAndArt.second.isNull()) {
                notLoadedArtIds.push_back(idAndArt.first);
                // go on and set the art, otherwise the server would be queried again and again next time
            }
            auto album = findByIdUnfiltered(idAndArt.first, myArtsLoadOffsetUnfiltered, myArtsLoadCount);
            if (album != nullptr) {
                album->setArt(unique_ptr<QPixmap>{new QPixmap{idAndArt.second}});
                loadedIdsAndArts.emplace(idAndArt);
            }
        }
    } else {
        for (auto& idAndArt: arts) {
            if (idAndArt.second.isNull()) {
                notLoadedArtIds.push_back(idAndArt.first);
                // go on and set the art, otherwise the server would be queried again and again next time
            }
            auto& album = getById(idAndArt.first);
            album.setArt(unique_ptr<QPixmap>{new QPixmap{idAndArt.second}});
            loadedIdsAndArts.emplace(idAndArt);
        }
    }

    return make_pair(loadedIdsAndArts, notLoadedArtIds);
}



Album* AlbumRepository::findById(const string& id, int filteredOffset, int count) const {
    auto filteredAlbumsData = myFilter->getFilteredData();
    auto albumDataIter = find_if(filteredAlbumsData.begin() + filteredOffset,
        filteredAlbumsData.begin() + filteredOffset + count,
        [&id](AlbumData& ad) {return ad.getId() == id;});
    if (albumDataIter == filteredAlbumsData.begin() + filteredOffset + count) {
        return nullptr;
    }
    return &(*albumDataIter).get().getAlbum();
}



Album* AlbumRepository::findByIdUnfiltered(const string& id, int offset, int count) const {
    auto albumDataIter = find_if(myData.begin() + offset, myData.begin() + offset + count,
        [&id](const unique_ptr<AlbumData>& ad) {return ad->getId() == id;});
    if (albumDataIter == myData.begin() + offset + count) {
        return nullptr;
    }
    return &(*albumDataIter).get()->getAlbum();
}

}
