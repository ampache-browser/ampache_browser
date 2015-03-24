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

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;
using namespace ui;



namespace application {

AmpacheBrowser::AmpacheBrowser(Ui& ui):
myUi(&ui) {
    myAlbumsModel = new QStandardItemModel{};
    ui.setAlbumsModel(*myAlbumsModel);

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
    myAmpacheService->readyAlbums += bind(&AmpacheBrowser::onReadyAlbums, this, _1);
    myUi->albumWindowRedraw += bind(&AmpacheBrowser::onAlbumWindowRedraw, this);
}



AmpacheBrowser::~AmpacheBrowser() {
    delete(myAmpacheService);
    delete(myAlbumsModel);
}



AmpacheBrowser::AmpacheBrowser(const AmpacheBrowser& other) = default;



AmpacheBrowser& AmpacheBrowser::operator=(const AmpacheBrowser& other) = default;



AmpacheBrowser::AmpacheBrowser(AmpacheBrowser&& other) = default;



AmpacheBrowser& AmpacheBrowser::operator=(AmpacheBrowser&& other) = default;



void AmpacheBrowser::onAlbumWindowRedraw() {
    myAmpacheService->requestAlbums(0, 10);
}



void AmpacheBrowser::onReadyAlbums(const vector<unique_ptr<Album>>& albums) {
    for (auto& album: albums) {
        auto item = new QStandardItem(QString::fromStdString(album->getName()));

        // SMELL: Compute size hint from pixmap size (or scale pixmap to size relative to this).
        item->setSizeHint(QSize(128, 128));

        item->setData(*(album->getArt()), Qt::ItemDataRole::DecorationRole);
        myAlbumsModel->appendRow(item);
    }
}

}
