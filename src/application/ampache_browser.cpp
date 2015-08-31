// ampache_browser.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <fstream>
#include <memory>

#include "ui/ui.h"
#include "domain/artist.h"
#include "application/models/album_model.h"
#include "application/models/artist_model.h"
#include "application/models/track_model.h"
#include "data/ampache_service.h"
#include "data/cache/cache.h"
#include "data/album_repository.h"
#include "data/artist_repository.h"
#include "data/track_repository.h"
#include "application/ampache_browser.h"

using namespace std;
using namespace placeholders;
using namespace domain;
using namespace data;
using namespace ui;



namespace application {

AmpacheBrowser::AmpacheBrowser(Ui& ui):
myUi(&ui) {
    string url;
    string user;
    string pass;
    ifstream creds{"../../elab/creds"};
    if (creds.is_open()) {
        getline(creds, url);
        getline(creds, user);
        getline(creds, pass);
        creds.close();
    }
    myAmpacheService = unique_ptr<AmpacheService>{new AmpacheService{url, user, pass}};
    myAmpacheService->connected += bind(&AmpacheBrowser::onConnected, this);
    myCache = unique_ptr<Cache>{new Cache{}};
}



void AmpacheBrowser::onConnected() {
    myAmpacheService->connected -= bind(&AmpacheBrowser::onConnected, this);

    myArtistRepository = unique_ptr<ArtistRepository>{new ArtistRepository{*myAmpacheService, *myCache}};
    myAlbumRepository = unique_ptr<AlbumRepository>{new AlbumRepository{*myAmpacheService, *myCache,
      *myArtistRepository}};
    myTrackRepository = unique_ptr<TrackRepository>{new TrackRepository{*myAmpacheService, *myCache,
      *myArtistRepository, *myAlbumRepository}};

    myArtistRepository->fullyLoaded += bind(&AmpacheBrowser::onArtistsFullyLoaded, this);
    myArtistModel = unique_ptr<ArtistModel>{new ArtistModel(*myArtistRepository)};
    myUi->setArtistModel(*myArtistModel);
}



void AmpacheBrowser::onArtistsFullyLoaded() {
    myArtistRepository->fullyLoaded -= bind(&AmpacheBrowser::onArtistsFullyLoaded, this);
    myAlbumRepository->fullyLoaded += bind(&AmpacheBrowser::onAlbumsFullyLoaded, this);
    myAlbumModel = unique_ptr<AlbumModel>{new AlbumModel(*myAlbumRepository)};
    myUi->setAlbumModel(*myAlbumModel);
}



void AmpacheBrowser::onAlbumsFullyLoaded() {
    myAlbumRepository->fullyLoaded -= bind(&AmpacheBrowser::onAlbumsFullyLoaded, this);
    myTrackRepository->fullyLoaded += bind(&AmpacheBrowser::onTracksFullyLoaded, this);
    myTrackModel = unique_ptr<TrackModel>{new TrackModel(*myTrackRepository)};
    myUi->setTrackModel(*myTrackModel);
}



void AmpacheBrowser::onTracksFullyLoaded() {
    myTrackRepository->fullyLoaded -= bind(&AmpacheBrowser::onTracksFullyLoaded, this);
    myAlbumRepository->setArtistIndex(myTrackRepository->getArtistAlbumIndex());

    myUi->artistsSelected += bind(&AmpacheBrowser::onArtistsSelected, this, _1);
    myUi->albumsSelected += bind(&AmpacheBrowser::onAlbumsSelected, this, _1);
    myUi->searchTriggered += bind(&AmpacheBrowser::onSearchTriggered, this, _1);
}



void AmpacheBrowser::onArtistsSelected(vector<string> ids) {
    if (ids.empty()) {
        myAlbumRepository->unsetFilter();
        myTrackRepository->unsetFilter();
    } else {
        vector<reference_wrapper<const Artist>> artists;
        for (auto id: ids) {
            auto& artist = myArtistRepository->getById(id);
            artists.push_back(artist);
        }
        myAlbumRepository->setArtistFilter(artists);
        myTrackRepository->setArtistFilter(artists);
    }
}



void AmpacheBrowser::onAlbumsSelected(vector<string> ids) {
    if (ids.empty()) {
        myTrackRepository->unsetFilter();
    } else {
        vector<reference_wrapper<const Album>> albums;
        for (auto id: ids) {
            auto& album = myAlbumRepository->getById(id);
            albums.push_back(album);
        }
        myTrackRepository->setAlbumFilter(albums);
    }
}



void AmpacheBrowser::onSearchTriggered(string searchText) {
    if (searchText.empty()) {
        myArtistRepository->unsetFilter();
        myAlbumRepository->unsetFilter();
        myTrackRepository->unsetFilter();
    } else {
        myArtistRepository->setNameFilter(searchText);
        myAlbumRepository->setNameFilter(searchText);
        myTrackRepository->setNameFilter(searchText);
    }
}

}
