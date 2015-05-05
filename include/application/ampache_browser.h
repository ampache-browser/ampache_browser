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



namespace application {

class AlbumModel;
class ArtistModel;
class AmpacheService;



class AmpacheBrowser {

public:
    AmpacheBrowser(ui::Ui& ui);

    ~AmpacheBrowser();

    AmpacheBrowser(const AmpacheBrowser& other);

    AmpacheBrowser& operator=(const AmpacheBrowser& other);

    AmpacheBrowser(AmpacheBrowser&& other);

    AmpacheBrowser& operator=(AmpacheBrowser&& other);

private:
    ui::Ui* myUi;
    AlbumModel* myAlbumModel = nullptr;
    ArtistModel* myArtistModel = nullptr;
    AmpacheService* myAmpacheService = nullptr;

    void onConnected();
    void onAlbumWindowRedraw();
    void onReadyAlbums(const std::vector<std::unique_ptr<domain::Album>>& albums);
};

}



#endif // AMPACHEBROWSER_H
