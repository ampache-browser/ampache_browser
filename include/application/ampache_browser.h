// ampache_browser.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef AMPACHEBROWSER_H
#define AMPACHEBROWSER_H



#include "models/album_model.h"
#include "models/artist_model.h"
#include "models/track_model.h"
#include "data/ampache_service.h"
#include "data/album_repository.h"
#include "data/artist_repository.h"
#include "data/track_repository.h"
#include "ui/ui.h"



namespace application {



class AmpacheBrowser {

public:
    AmpacheBrowser(ui::Ui& ui);

    AmpacheBrowser(const AmpacheBrowser& other) = delete;

    AmpacheBrowser& operator=(const AmpacheBrowser& other) = delete;

private:
    ui::Ui* const myUi;

    std::unique_ptr<data::AmpacheService> myAmpacheService;
    std::unique_ptr<data::AlbumRepository> myAlbumRepository;
    std::unique_ptr<data::ArtistRepository> myArtistRepository;
    std::unique_ptr<data::TrackRepository> myTrackRepository;

    std::unique_ptr<AlbumModel> myAlbumModel;
    std::unique_ptr<ArtistModel> myArtistModel;
    std::unique_ptr<TrackModel> myTrackModel;

    void onConnected();
    void onArtistsFullyLoaded();
    void onAlbumsFullyLoaded();
    void onTracksFullyLoaded();
    void onArtistSelected(std::string id);
};

}



#endif // AMPACHEBROWSER_H
