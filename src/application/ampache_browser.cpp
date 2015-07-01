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
    myUi->artistSelected += bind(&AmpacheBrowser::onArtistSelected, this, _1);
}



void AmpacheBrowser::onConnected() {
    myAlbumRepository = unique_ptr<AlbumRepository>{new AlbumRepository{*myAmpacheService}};
    myArtistRepository = unique_ptr<ArtistRepository>{new ArtistRepository{*myAmpacheService}};
    myTrackRepository = unique_ptr<TrackRepository>{new TrackRepository{*myAmpacheService}};
    myAlbumModel = unique_ptr<AlbumModel>{new AlbumModel(*myAlbumRepository)};
    myArtistModel = unique_ptr<ArtistModel>{new ArtistModel(*myArtistRepository)};
    myTrackModel = unique_ptr<TrackModel>{new TrackModel(*myTrackRepository)};
    myUi->setAlbumModel(*myAlbumModel);
    myUi->setArtistModel(*myArtistModel);
    myUi->setTrackModel(*myTrackModel);
}



void AmpacheBrowser::onArtistSelected(string id) {
    // TODO
    cout << id << endl;
}

}
