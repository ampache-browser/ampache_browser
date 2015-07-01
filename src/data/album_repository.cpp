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

#include "domain/artist.h"
#include "data/ampache_service.h"
#include "album_data.h"
#include "data/artist_repository.h"
#include "data/album_repository.h"

using namespace std;
using namespace placeholders;
using namespace domain;



namespace data {

AlbumRepository::AlbumRepository(AmpacheService& ampacheService):
myAmpacheService(ampacheService) {
    // SMELL: Is there a need to subscribe to myAmpacheService.connected? (Vector size can be reserved and also (max.)
    // number of albums initialized.)
    myAmpacheService.readyAlbums += bind(&AlbumRepository::onReadyAlbums, this, _1);
    myAmpacheService.readyAlbumArts += bind(&AlbumRepository::onReadyArts, this, _1);
}



bool AlbumRepository::load(int offset, int limit) {
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



Album& AlbumRepository::get(int offset) const {
    return myAlbumsData[offset]->getAlbum();
}



// Album& AlbumRepository::getById(const string& id) {
//     auto albumDataIter = find_if(myAlbumsData.begin(), myAlbumsData.end(),
//         [id](unique_ptr<AlbumData>& ad) {return ad->getId() == id;});
//     return (*albumDataIter)->getAlbum();
// }



vector<reference_wrapper<Album>> AlbumRepository::getByArtist(const Artist& artist) const {
    vector<reference_wrapper<Album>> filteredAlbums;
    for (auto& albumData: myAlbumsData) {
        auto& album = albumData->getAlbum();
        if (album.getArtist() ==  artist) {
            filteredAlbums.push_back(album);
        }
    }
    return filteredAlbums;
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



bool AlbumRepository::isLoaded(int offset, int limit) const {
    uint end = offset + limit;
    return (myAlbumsData.size() >= end) && all_of(myAlbumsData.begin() + offset, myAlbumsData.begin() + offset + limit,
        [](const unique_ptr<AlbumData>& ad) {return ad != nullptr;});
}



// SMELL: Expensive - hold a count as instance variable instead?
// int AlbumRepository::count() const {
//     return count_if(myAlbumsData.begin(), myAlbumsData.end(),
//         [](const unique_ptr<AlbumData>& ad) {return ad != nullptr;});
// }



int AlbumRepository::maxCount() const {
    return myAmpacheService.numberOfAlbums();
}



void AlbumRepository::onReadyAlbums(vector<unique_ptr<AlbumData>>& albumsData) {
    uint offset = myLoadOffset;
    auto end = offset + albumsData.size();
    if (end > myAlbumsData.size()) {
        // SMELL: Check how the elements are initialized (nullptr?).
        myAlbumsData.resize(end);
    }

    for (auto& albumData: albumsData) {
        myAlbumsData[offset++] = move(albumData);
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, albumsData.size()};
    myLoadOffset = -1;
    loaded(offsetAndLimit);
}



void AlbumRepository::onReadyArts(std::map<std::string, QPixmap>& arts) {
    uint offset = myArtsLoadOffset;

    for (auto urlAndArt: arts) {
        myAlbumsData[offset++]->getAlbum().setArt(unique_ptr<QPixmap>{new QPixmap{urlAndArt.second}});
    }

    auto offsetAndLimit = pair<int, int>{myLoadOffset, arts.size()};
    myArtsLoadOffset = -1;
    artsLoaded(offsetAndLimit);
}

}
