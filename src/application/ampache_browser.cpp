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
#include "application/ampache_browser.h"
#include "ui/ui.h"
#include "application/album_model.h"

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
    delete(myAlbumsModel);
}



AmpacheBrowser::AmpacheBrowser(const AmpacheBrowser& other) = default;



AmpacheBrowser& AmpacheBrowser::operator=(const AmpacheBrowser& other) = default;



AmpacheBrowser::AmpacheBrowser(AmpacheBrowser&& other) = default;



AmpacheBrowser& AmpacheBrowser::operator=(AmpacheBrowser&& other) = default;



void AmpacheBrowser::onConnected() {
    myAlbumsModel = new AlbumModel{*myAmpacheService};
    myUi->setAlbumsModel(*myAlbumsModel);
}

}
