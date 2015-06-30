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
using namespace infrastructure;
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
    myAmpacheService = new AmpacheService{url, user, pass};
    myAmpacheService->connected += bind(&AmpacheBrowser::onConnected, this);
    myUi->artistSelected += bind(&AmpacheBrowser::onArtistSelected, this, _1);
}



AmpacheBrowser::~AmpacheBrowser() {
    delete(myAlbumModel);
    delete(myArtistModel);
    delete(myTrackModel);
    delete(myAlbumRepository);
    delete(myArtistRepository);
    delete(myTrackRepository);
    delete(myAmpacheService);
}



AmpacheBrowser::AmpacheBrowser(const AmpacheBrowser&) = default;



AmpacheBrowser& AmpacheBrowser::operator=(const AmpacheBrowser&) = default;



AmpacheBrowser::AmpacheBrowser(AmpacheBrowser&&) = default;



AmpacheBrowser& AmpacheBrowser::operator=(AmpacheBrowser&&) = default;



void AmpacheBrowser::onConnected() {
    myAlbumRepository = new AlbumRepository{*myAmpacheService};
    myArtistRepository = new ArtistRepository{*myAmpacheService};
    myTrackRepository = new TrackRepository{*myAmpacheService};
    myAlbumModel = new AlbumModel(*myAlbumRepository);
    myArtistModel = new ArtistModel(*myArtistRepository);
    myTrackModel = new TrackModel(*myTrackRepository);
    myUi->setAlbumModel(*myAlbumModel);
    myUi->setArtistModel(*myArtistModel);
    myUi->setTrackModel(*myTrackModel);
}



void AmpacheBrowser::onArtistSelected(string id) {
    // TODO
    cout << id << endl;
}

}
