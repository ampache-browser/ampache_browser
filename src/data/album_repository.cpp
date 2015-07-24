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

// TODO: www Rework to forbid filter setting until fully loaded. - Maybe no - loading stuff will always use full
// offset (non-filtered).  get() will use filtered offset.
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



// vector<reference_wrapper<Album>> AlbumRepository::get(int offset, int limit) {
//     vector<reference_wrapper<Album>> albums;
//     for (auto idx = offset; idx >= offset + limit; idx++) {
//         albums.push_back(myAlbumsData[idx]->getAlbum());
//     }
//     return albums;
// }



Album& AlbumRepository::get(int filteredOffset) const {
    AlbumData& albumData = myAlbumDataReferences[filteredOffset];
    return albumData.getAlbum();
}



// Album& AlbumRepository::getById(const string& id) {
//     auto albumDataIter = find_if(myAlbumsData.begin(), myAlbumsData.end(),
//         [id](unique_ptr<AlbumData>& ad) {return ad->getId() == id;});
//     return (*albumDataIter)->getAlbum();
// }



// vector<reference_wrapper<Album>> AlbumRepository::getByArtist(const Artist& artist) const {
//     vector<reference_wrapper<Album>> filteredAlbums;
//     for (auto& albumData: myAlbumsData) {
//         auto& album = albumData->getAlbum();
//         if (album.getArtist() ==  artist) {
//             filteredAlbums.push_back(album);
//         }
//     }
//
//     return filteredAlbums;
// }



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



void AlbumRepository::populateArtists(const ArtistRepository& artistRepository) {
    for (auto& albumData: myAlbumsData) {
        auto& album = albumData->getAlbum();
        album.setArtist(artistRepository.getById(albumData->getArtistId()));
    }
}



bool AlbumRepository::isLoaded(int filteredOffset, int limit) const {
    uint end = filteredOffset + limit;

    // TODO: Check, if &ad != nullptr works.
    return (myAlbumDataReferences.size() >= end) && all_of(myAlbumDataReferences.begin() + filteredOffset,
        myAlbumDataReferences.begin() + filteredOffset + limit, [](const AlbumData& ad) {return &ad != nullptr;});
}



// SMELL: Expensive - hold a count as instance variable instead?
// int AlbumRepository::count() const {
//     return count_if(myAlbumsData.begin(), myAlbumsData.end(),
//         [](const unique_ptr<AlbumData>& ad) {return ad != nullptr;});
// }



int AlbumRepository::maxCount() const {
    if (myCurrentArtistFilter != nullptr) {
        return myArtistRepository.getArtistData(*myCurrentArtistFilter).getNumberOfAlbums();
    }
    return myAmpacheService.numberOfAlbums();
}



void AlbumRepository::setArtistFilter(const Artist& artist) {
    unsetArtistFilter();
    myCurrentArtistFilter = &artist;
    (*myArtistIndex)[*myCurrentArtistFilter].swap(myAlbumDataReferences);

    bool b = false;
    filterChanged(b);
}



void AlbumRepository::unsetArtistFilter() {
    if (myCurrentArtistFilter == nullptr) {
        return;
    }
    myAlbumDataReferences.swap((*myArtistIndex)[*myCurrentArtistFilter]);
    myCurrentArtistFilter = nullptr;

    bool b = false;
    filterChanged(b);
}



void AlbumRepository::setArtistIndex(unique_ptr<ArtistAlbumVectorIndex> artistIndex) {
    myArtistIndex = move(artistIndex);
}



void AlbumRepository::onReadyAlbums(vector<unique_ptr<AlbumData>>& albumsData) {
    auto storedCurrentArtistFilter = myCurrentArtistFilter;
    unsetArtistFilter();

    uint offset = myLoadOffset;
    auto end = offset + albumsData.size();
    if (end > myAlbumsData.size()) {
        // SMELL: Check how the elements are initialized (nullptr?).
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
    if (storedCurrentArtistFilter != nullptr) {
        setArtistFilter(*storedCurrentArtistFilter);
    }

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

}
