// ampache_browser.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef AMPACHEBROWSER_H
#define AMPACHEBROWSER_H



#include "models/album_model.h"
#include "models/artist_model.h"
#include "models/track_model.h"
#include "data/providers/ampache.h"
#include "data/providers/cache.h"
#include "data/repositories/album_repository.h"
#include "data/repositories/artist_repository.h"
#include "data/repositories/track_repository.h"
#include "data/indices.h"
#include "ui/selected_items.h"
#include "application/data_loader.h"

namespace ui {
class Ui;
}


namespace application {

class DataLoader;



/**
 * @brief The application.
 *
 * Class represents the application and contains the application logic.
 */
class AmpacheBrowser {

public:
    /**
     * @brief Constructor.
     *
     * @param ui The user interface.
     */
    explicit AmpacheBrowser(ui::Ui& ui);

    /**
     * @brief Destructor.
     *
     * @note Should be called only after ::terminated event was fired due to gracefull termination of asynchronous
     * operations.
     */
    ~AmpacheBrowser();

    AmpacheBrowser(const AmpacheBrowser& other) = delete;

    AmpacheBrowser& operator=(const AmpacheBrowser& other) = delete;

    /**
     * @brief Fired after all asynchronous operations has been terminated.
     *
     * @sa requestTermination()
     */
    infrastructure::Event<void> terminated{};

    /**
     * @brief Request to terminate the application.
     *
     * This method should be used to end the application gracefully.  It signals to terminate all asynchronous
     * operations and once they are terminated it fires ::terminated event.  The consumer of the event then can delete
     * the instance.
     */
    void requestTermination();

private:
    const std::string SETTINGS_SECTION = "ampache_browser";
    const std::string SETTINGS_USE_DEMO_SERVER = "use_demo_server";
    const std::string SETTINGS_SERVER_URL = "server_url";
    const std::string SETTINGS_USER_NAME = "user_name";
    const std::string SETTINGS_PASSWORD_HASH = "password_hash";

    std::unique_ptr<DataLoader> myDataLoader = nullptr;

    std::unique_ptr<data::Ampache> myAmpache = nullptr;
    std::unique_ptr<data::Cache> myCache = nullptr;
    std::unique_ptr<data::Indices> myIndices = nullptr;
    std::unique_ptr<data::ArtistRepository> myArtistRepository = nullptr;
    std::unique_ptr<data::AlbumRepository> myAlbumRepository = nullptr;
    std::unique_ptr<data::TrackRepository> myTrackRepository = nullptr;

    std::unique_ptr<AlbumModel> myAlbumModel = nullptr;
    std::unique_ptr<ArtistModel> myArtistModel = nullptr;
    std::unique_ptr<TrackModel> myTrackModel = nullptr;

    ui::Ui* const myUi = nullptr;

    bool mySettingsUpdated = false;

    ui::SelectedItems myPlayIds{};

    void onDataLoaderFinished(LoadingResult loadingResult);
    void onPlayTriggered(ui::SelectedItems& selectedItems);
    void onCreatePlaylistTriggered(ui::SelectedItems& selectedItems);
    void onAddToPlaylistTriggered(ui::SelectedItems& selectedItems);
    void onArtistsSelected(const std::vector<std::string>& ids);
    void onAlbumsSelected(const std::pair<std::vector<std::string>, std::vector<std::string>>& albumAndArtistIds);
    void onSearchTriggered(const std::string& searchText);

    void onPlayOrCreateReadySession(bool error);
    void onAddReadySession(bool error);

    void onSettingsUpdated(std::tuple<bool, std::string, std::string, std::string> settings);

    void initializeAndLoad();
    void initializeDependencies();
    void uninitializeDependencies();

    Index<PlaylistAddItem> createPlaylistItems(bool error);
    void setArtistFilters(const std::vector<std::string>& ids);
};

}



#endif // AMPACHEBROWSER_H
