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
#include "ui/ui.h"
#include "application/data_loader.h"



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

    bool myIsArtistDataRequestAborted = false;
    bool myIsAlbumDataRequestAborted = false;
    bool myIsTrackDataRequestAborted = false;

    std::vector<std::string> myPlayIds{};

    void onDataLoaderFinished(LoadingResult loadingResult);
    void onPlayTriggered(const std::vector<std::string>& ids);
    void onCreatePlaylistTriggered(const std::vector<std::string>& ids);
    void onAddToPlaylistTriggered(const std::vector<std::string>& ids);
    void onPlayOrCreateReadySession(bool error);
    void onAddReadySession(bool error);
    void onArtistsSelected(const std::vector<std::string>& ids);
    void onAlbumsSelected(const std::vector<std::string>& ids);
    void onSearchTriggered(const std::string& searchText);

    Index<PlaylistAddItem> createPlaylistItems(bool error);
};

}



#endif // AMPACHEBROWSER_H
