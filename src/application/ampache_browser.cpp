// ampache_browser.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <iostream>

#include <fstream>
#include <memory>

#include "ui/ui.h"
#include "application/models/album_model.h"
#include "application/models/artist_model.h"
#include "application/models/track_model.h"
#include "data/ampache_service.h"
#include "data/album_repository.h"
#include "data/artist_repository.h"
#include "data/track_repository.h"
#include "application/ampache_browser.h"

using namespace std;
using namespace placeholders;
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
}



void AmpacheBrowser::onConnected() {
    myAmpacheService->connected -= bind(&AmpacheBrowser::onConnected, this);

    myArtistRepository = unique_ptr<ArtistRepository>{new ArtistRepository{*myAmpacheService}};
    myAlbumRepository = unique_ptr<AlbumRepository>{new AlbumRepository{*myAmpacheService, *myArtistRepository}};
    myTrackRepository = unique_ptr<TrackRepository>{new TrackRepository{*myAmpacheService, *myArtistRepository,
        *myAlbumRepository}};

    myArtistRepository->fullyLoaded += bind(&AmpacheBrowser::onArtistsFullyLoaded, this);
    myArtistModel = unique_ptr<ArtistModel>{new ArtistModel(*myArtistRepository)};
    myUi->setArtistModel(*myArtistModel);
}



void AmpacheBrowser::onArtistSelected(string id) {
    // TODO: Set filter on album (and track) repository.
    auto& artist = myArtistRepository->getById(id);
    myAlbumRepository->setArtistFilter(artist);
    myTrackRepository->setArtistFilter(artist);
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
    
    myUi->artistSelected += bind(&AmpacheBrowser::onArtistSelected, this, _1);
}

}
