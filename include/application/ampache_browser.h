// ampache_browser.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef AMPACHEBROWSER_H
#define AMPACHEBROWSER_H



#include <QtGui/QStandardItemModel>
#include <vector>
#include <memory>
#include "domain/album.h"
#include "ui/ui.h"
#include "application/album_model.h"

// SMELL: How to organize file structure better?
#include "src/application/ampache_service.h"

using namespace std;
using namespace domain;
using namespace ui;



namespace application {

class AmpacheBrowser {

public:
    AmpacheBrowser(Ui& ui);

    ~AmpacheBrowser();

    AmpacheBrowser(const AmpacheBrowser& other);

    AmpacheBrowser& operator=(const AmpacheBrowser& other);

    AmpacheBrowser(AmpacheBrowser&& other);

    AmpacheBrowser& operator=(AmpacheBrowser&& other);

private:
    Ui* myUi;
    AlbumModel* myAlbumsModel;
    AmpacheService* myAmpacheService;

    void onConnected();
    void onAlbumWindowRedraw();
    void onReadyAlbums(const vector<unique_ptr<Album>>& albums);
};

}



#endif // AMPACHEBROWSER_H
