// ampache_browser.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <fstream>
#include <memory>
#include <QtGui/QStandardItemModel>
#include <QtCore/QString>
#include <QtCore/QList>
#include "domain/album.h"
#include "ui/ui.h"
#include "application/models/album_model.h"
#include "application/models/artist_model.h"
#include "ampache_service.h"
#include "application/ampache_browser.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;
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
}



AmpacheBrowser::~AmpacheBrowser() {
    delete(myAmpacheService);
    delete(myAlbumModel);
    delete(myArtistModel);
}



AmpacheBrowser::AmpacheBrowser(const AmpacheBrowser&) = default;



AmpacheBrowser& AmpacheBrowser::operator=(const AmpacheBrowser&) = default;



AmpacheBrowser::AmpacheBrowser(AmpacheBrowser&&) = default;



AmpacheBrowser& AmpacheBrowser::operator=(AmpacheBrowser&&) = default;



void AmpacheBrowser::onConnected() {
    myAlbumModel = new AlbumModel(*myAmpacheService);
    myArtistModel = new ArtistModel(*myAmpacheService);
    myUi->setAlbumModel(*myAlbumModel);
    myUi->setArtistModel(*myArtistModel);
}

}
