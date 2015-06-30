// ampache_browser.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef AMPACHEBROWSER_H
#define AMPACHEBROWSER_H



#include "data/ampache_service.h"
#include "data/album_repository.h"
#include "data/artist_repository.h"
#include "data/track_repository.h"
#include "ui/ui.h"



namespace application {

class AlbumModel;
class ArtistModel;
class TrackModel;



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

    data::AmpacheService* myAmpacheService = nullptr;
    data::AlbumRepository* myAlbumRepository = nullptr;
    data::ArtistRepository* myArtistRepository = nullptr;
    data::TrackRepository* myTrackRepository = nullptr;

    AlbumModel* myAlbumModel = nullptr;
    ArtistModel* myArtistModel = nullptr;
    TrackModel* myTrackModel = nullptr;

    void onConnected();
    void onArtistSelected(std::string id);
};

}



#endif // AMPACHEBROWSER_H
