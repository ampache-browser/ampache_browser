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
#include "data/cache/cache.h"
#include "data/album_repository.h"
#include "data/artist_repository.h"
#include "data/track_repository.h"
#include "data/indices.h"
#include "ui/ui.h"



namespace application {

class AmpacheBrowser {

public:
    AmpacheBrowser(ui::Ui& ui);

    AmpacheBrowser(const AmpacheBrowser& other) = delete;

    AmpacheBrowser& operator=(const AmpacheBrowser& other) = delete;

private:
    ui::Ui* const myUi = nullptr;

    std::unique_ptr<data::AmpacheService> myAmpacheService = nullptr;
    std::unique_ptr<data::Cache> myCache = nullptr;
    std::unique_ptr<data::AlbumRepository> myAlbumRepository = nullptr;
    std::unique_ptr<data::ArtistRepository> myArtistRepository = nullptr;
    std::unique_ptr<data::TrackRepository> myTrackRepository = nullptr;
    std::unique_ptr<data::Indices> myIndices = nullptr;

    std::unique_ptr<AlbumModel> myAlbumModel = nullptr;
    std::unique_ptr<ArtistModel> myArtistModel = nullptr;
    std::unique_ptr<TrackModel> myTrackModel = nullptr;

    void onConnected();
    void onArtistsFullyLoaded();
    void onAlbumsFullyLoaded();
    void onTracksFullyLoaded();
    void onArtistsSelected(std::vector<std::string> id);
    void onAlbumsSelected(std::vector<std::string> id);
    void onSearchTriggered(std::string searchText);
};

}



#endif // AMPACHEBROWSER_H
