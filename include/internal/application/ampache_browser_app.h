// ampache_browser_app.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef AMPACHEBROWSERAPP_H
#define AMPACHEBROWSERAPP_H



#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "ui/selected_items.h"
#include "application/data_loader.h"

class QWidget;

namespace data {
class Ampache;
class Cache;
class Indices;
class ArtistRepository;
class AlbumRepository;
class TrackRepository;
}

namespace ui {
class Ui;
}



namespace application {

class ArtistModel;
class AlbumModel;
class TrackModel;
class Filtering;
class SettingsInternal;



/**
 * @brief The application logic.
 */
class AmpacheBrowserApp {

public:
    /**
     * @brief Creates the instance of main application class.
     *
     * @param settingsInternal Application settings.
     */
    explicit AmpacheBrowserApp(SettingsInternal& settingsInternal);

    /**
     * @brief Destroys the instance.
     *
     * @warning The finishRequest() method should be called first and the instance destroyed after the callback
     * finishedCb is called.
     */
    ~AmpacheBrowserApp();

    /**
     * @brief Sets a callback that will be called after the user performed "play" action.
     *
     * @param callback Function that shall be set as callback.
     */
    void connectPlay(std::function<void(std::vector<std::string>)> callback);

    /**
     * @brief Sets a callback that will be called after the user performed "create playlist" action.
     *
     * @param callback Function that shall be set as callback.
     */
    void connectCreatePlaylist(std::function<void(std::vector<std::string>)> callback);

    /**
     * @brief Sets a callback that will be called after the user performed "add to playlist" action.
     *
     * @param callback Function that shall be set as callback.
     */
    void connectAddToPlaylist(std::function<void(std::vector<std::string>)> callback);

    /**
     * @brief Gets main window widget.
     *
     * @note Widget is not created until run() is called.
     *
     * @return The main window widget or nullptr if not created yet.
     */
    QWidget* getMainWidget() const;

    /**
     * @brief Creates UI window, connects to the server and starts reading data.
     *
     * @warning The SettingsInternal instance passed to the constructor has to be initialized (all settings have to
     * be set) prior to calling this method.
     */
    void run();

    /**
     * @brief Request to finish/stop the application.
     *
     * This method should be used to stop the application gracefully.  It signals to terminate all asynchronous
     * operations and once they are terminated it calls the passed callback function.  The callback can delete
     * the instance then.
     *
     * The implementation must ensure that no instance variable is accessed after the callback returns because
     * the instance might be already destroyed.
     */
    void finishRequest(std::function<void()> finishedCb);

private:
    SettingsInternal& mySettingsInternal;

    std::function<void(std::vector<std::string>)> myPlayCb = [](std::vector<std::string>) { };
    std::function<void(std::vector<std::string>)> myCreatePlaylistCb = [](std::vector<std::string>) { };
    std::function<void(std::vector<std::string>)> myAddToPlaylistCb = [](std::vector<std::string>) { };
    std::function<void()> myFinishedCb;

    std::unique_ptr<ui::Ui> myUi;
    std::unique_ptr<application::DataLoader> myDataLoader;
    std::unique_ptr<data::Ampache> myAmpache;
    std::unique_ptr<data::Cache> myCache;
    std::unique_ptr<data::Indices> myIndices;
    std::unique_ptr<data::ArtistRepository> myArtistRepository;
    std::unique_ptr<data::AlbumRepository> myAlbumRepository;
    std::unique_ptr<data::TrackRepository> myTrackRepository;

    std::unique_ptr<application::ArtistModel> myArtistModel;
    std::unique_ptr<application::AlbumModel> myAlbumModel;
    std::unique_ptr<application::TrackModel> myTrackModel;

    std::unique_ptr<application::Filtering> myFiltering;

    ui::SelectedItems myPlayIds;

    void onDataLoaderFinished(application::LoadingResult loadingResult);
    void onApplySettingsDataLoaderAborted();
    void onFinishRequestDataLoaderAborted();
    void onPlayTriggered(ui::SelectedItems& selectedItems);
    void onCreatePlaylistTriggered(ui::SelectedItems& selectedItems);
    void onAddToPlaylistTriggered(ui::SelectedItems& selectedItems);

    void onPlayTriggeredAmpacheReadySession(bool error);
    void onCreatePlaylistTriggeredAmpacheReadySession(bool error);
    void onAddToPlaylistTriggeredAmpacheReadySession(bool error);

    void onSettingsUpdated(std::tuple<bool, std::string, std::string, std::string> settings);

    void initializeAndLoad();
    void initializeDependencies();
    void uninitializeDependencies();
    void applySettings();
    std::vector<std::string> createPlaylistItems(bool error);
};

}



#endif // AMPACHEBROWSERAPP_H
