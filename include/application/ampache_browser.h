// ampache_browser.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef AMPACHEBROWSER_H
#define AMPACHEBROWSER_H



# include "ui/ui.h"



namespace application {

class AlbumModel;
class ArtistModel;
class TrackModel;
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
    TrackModel* myTrackModel = nullptr;
    AmpacheService* myAmpacheService = nullptr;

    void onConnected();
};

}



#endif // AMPACHEBROWSER_H
