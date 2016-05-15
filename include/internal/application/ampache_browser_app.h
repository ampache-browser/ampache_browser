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
    explicit AmpacheBrowserApp(SettingsInternal* const settings);

    ~AmpacheBrowserApp();

    void connectPlay(std::function<void(std::vector<std::string>)> callback);

    void connectCreatePlaylist(std::function<void(std::vector<std::string>)> callback);

    void connectAddToPlaylist(std::function<void(std::vector<std::string>)> callback);

    QWidget* getMainWidget() const;

    void requestTermination(std::function<void()> terminatedCb);

private:
    std::function<void(std::vector<std::string>)> myPlayCb = [](std::vector<std::string>) { };
    std::function<void(std::vector<std::string>)> myCreatePlaylistCb = [](std::vector<std::string>) { };
    std::function<void(std::vector<std::string>)> myAddToPlaylistCb = [](std::vector<std::string>) { };
    std::function<void()> myTerminatedCb;

    SettingsInternal* const mySettings = nullptr;

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
    void onRequestTerminationDataLoaderAborted();
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
