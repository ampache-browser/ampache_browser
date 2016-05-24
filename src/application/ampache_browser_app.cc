// ampache_browser_app.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <string>

#include <libaudcore/i18n.h>
#include <QCryptographicHash>

#include "infrastructure/event/delegate.h"
#include "infrastructure/logging/logging.h"
#include "data/providers/ampache.h"
#include "data/providers/cache.h"
#include "data/indices.h"
#include "data/repositories/album_repository.h"
#include "data/repositories/artist_repository.h"
#include "data/repositories/track_repository.h"
#include "ui/ui.h"
#include "application/models/artist_model.h"
#include "application/models/album_model.h"
#include "application/models/track_model.h"
#include "application/data_loader.h"
#include "application/settings_internal.h"
#include "ampache_browser/settings.h"
#include "ampache_browser/ampache_browser.h"
#include "filtering.h"
#include "application/ampache_browser_app.h"

using namespace std;
using namespace ampache_browser;
using namespace infrastructure;
using namespace data;
using namespace ui;



namespace application {

AmpacheBrowserApp::AmpacheBrowserApp(SettingsInternal& settingsInternal):
mySettingsInternal(settingsInternal) {
}



AmpacheBrowserApp::~AmpacheBrowserApp() {
}



void AmpacheBrowserApp::connectPlay(function<void(vector<string>)> callback) {
    myPlayCb = callback;
}



void AmpacheBrowserApp::connectCreatePlaylist(function<void(vector<string>)> callback) {
    myCreatePlaylistCb = callback;
}



void AmpacheBrowserApp::connectAddToPlaylist(function<void(vector<string>)> callback) {
    myAddToPlaylistCb = callback;
}



QWidget* AmpacheBrowserApp::getMainWidget() const {
    return myUi != nullptr ? myUi->getMainWidget() : nullptr;
}



void AmpacheBrowserApp::run() {
    LOG_INF("Starting...");

    myUi = unique_ptr<Ui>(new Ui{});
    initializeAndLoad();
}



void AmpacheBrowserApp::finishRequest(function<void()> finishedCb) {
    LOG_INF("Finish request.");

    myFinishedCb = finishedCb;

    // SMELL: Handshake with the server can be in progress.  Unsubscribing from event here is most likely not enough.
    myAmpache->readySession -= DELEGATE1(&AmpacheBrowserApp::onPlayTriggeredAmpacheReadySession, bool);

    myDataLoader->aborted += DELEGATE0(&AmpacheBrowserApp::onFinishRequestDataLoaderAborted);
    myDataLoader->abort();
}



void AmpacheBrowserApp::onDataLoaderFinished(LoadingResult loadingResult) {
    switch (loadingResult) {
        case LoadingResult::Error:
            myUi->showNotification(_("Error while reading data!"));
            break;
        case LoadingResult::NoConnectionNoCache:
        case LoadingResult::SuccessNoConnection:
            myUi->showNotification(_("Unable to connect to server."));
            break;
        default:
            myUi->showNotification(_("Loaded."));
    }
}



void AmpacheBrowserApp::onApplySettingsDataLoaderAborted() {
    myDataLoader->aborted -= DELEGATE0(&AmpacheBrowserApp::onApplySettingsDataLoaderAborted);
    uninitializeDependencies();
    initializeAndLoad();
}



void AmpacheBrowserApp::onFinishRequestDataLoaderAborted() {
    myDataLoader->aborted -= DELEGATE0(&AmpacheBrowserApp::onFinishRequestDataLoaderAborted);
    uninitializeDependencies();
    myUi = nullptr;
    myFinishedCb();
}



void AmpacheBrowserApp::onPlayTriggered(SelectedItems& selectedItems) {
    myPlayIds = move(selectedItems);
    myAmpache->readySession += DELEGATE1(&AmpacheBrowserApp::onPlayTriggeredAmpacheReadySession, bool);
    if (myDataLoader->isLoadingInProgress()) {
        onPlayTriggeredAmpacheReadySession(false);
    } else {
        myAmpache->refreshSession();
    }
}



void AmpacheBrowserApp::onCreatePlaylistTriggered(SelectedItems& selectedItems) {
    myPlayIds = move(selectedItems);
    myAmpache->readySession += DELEGATE1(&AmpacheBrowserApp::onCreatePlaylistTriggeredAmpacheReadySession, bool);
    if (myDataLoader->isLoadingInProgress()) {
        onCreatePlaylistTriggeredAmpacheReadySession(false);
    } else {
        myAmpache->refreshSession();
    }

}



void AmpacheBrowserApp::onAddToPlaylistTriggered(SelectedItems& selectedItems) {
    myPlayIds = move(selectedItems);
    myAmpache->readySession += DELEGATE1(&AmpacheBrowserApp::onAddToPlaylistTriggeredAmpacheReadySession, bool);
    if (myDataLoader->isLoadingInProgress()) {
        onAddToPlaylistTriggeredAmpacheReadySession(false);
    } else {
        myAmpache->refreshSession();
    }
}



void AmpacheBrowserApp::onPlayTriggeredAmpacheReadySession(bool error) {
    myAmpache->readySession -= DELEGATE1(&AmpacheBrowserApp::onPlayTriggeredAmpacheReadySession, bool);
    auto playlistItems = createPlaylistItems(error);
    myPlayCb(playlistItems);
}



void AmpacheBrowserApp::onCreatePlaylistTriggeredAmpacheReadySession(bool error) {
    myAmpache->readySession -= DELEGATE1(&AmpacheBrowserApp::onCreatePlaylistTriggeredAmpacheReadySession, bool);
    auto playlistItems = createPlaylistItems(error);
    myCreatePlaylistCb(playlistItems);
}



void AmpacheBrowserApp::onAddToPlaylistTriggeredAmpacheReadySession(bool error) {
    myAmpache->readySession -= DELEGATE1(&AmpacheBrowserApp::onAddToPlaylistTriggeredAmpacheReadySession, bool);
    auto playlistItems = createPlaylistItems(error);
    myAddToPlaylistCb(playlistItems);
}



void AmpacheBrowserApp::onSettingsUpdated(tuple<bool, string, string, string> settings) {
    mySettingsInternal.beginGroupSet();
    mySettingsInternal.setBool(Settings::USE_DEMO_SERVER, get<0>(settings));
    mySettingsInternal.setString(Settings::SERVER_URL, get<1>(settings));
    mySettingsInternal.setString(Settings::USER_NAME, get<2>(settings));
    auto passwordHash = QCryptographicHash::hash(QByteArray{get<3>(settings).c_str()},
        QCryptographicHash::Sha256).toHex().data();
    mySettingsInternal.setString(Settings::PASSWORD_HASH, passwordHash);
    mySettingsInternal.endGroupSet();

    applySettings();
}



void AmpacheBrowserApp::initializeAndLoad() {
    auto useDemoServer = mySettingsInternal.getBool(Settings::USE_DEMO_SERVER);
    auto serverUrl = mySettingsInternal.getString(Settings::SERVER_URL);
    auto userName = mySettingsInternal.getString(Settings::USER_NAME);
    auto passwordHash = mySettingsInternal.getString(Settings::PASSWORD_HASH);

    myUi->populateSettings(useDemoServer, serverUrl, userName);

    if (useDemoServer) {
        serverUrl = "http://demo.ampache-browser.org/";
        userName = "demo";
        passwordHash = "1b2e48536c91351b5ea0a32a3bbaa0fc1ef9de6bc20b254a9a7e22043a211e33";
    }

    myAmpache = unique_ptr<Ampache>{new Ampache{serverUrl, userName, passwordHash}};
    myCache = unique_ptr<Cache>{new Cache{serverUrl, userName}};
    myIndices = unique_ptr<Indices>{new Indices{}};

    initializeDependencies();

    myUi->showNotification(_("Loading..."));
    myDataLoader->load();
}



void AmpacheBrowserApp::initializeDependencies() {
    myArtistRepository = unique_ptr<ArtistRepository>{new ArtistRepository{*myAmpache, *myCache, *myIndices}};
    myAlbumRepository = unique_ptr<AlbumRepository>{new AlbumRepository{*myAmpache, *myCache, *myIndices,
        myArtistRepository.get()}};
    myTrackRepository = unique_ptr<TrackRepository>{new TrackRepository{*myAmpache, *myCache, *myIndices,
        myArtistRepository.get(), myAlbumRepository.get()}};

    myArtistModel = unique_ptr<ArtistModel>{new ArtistModel{myArtistRepository.get()}};
    myAlbumModel = unique_ptr<AlbumModel>{new AlbumModel{myAlbumRepository.get()}};
    myTrackModel = unique_ptr<TrackModel>{new TrackModel{myTrackRepository.get()}};

    myUi->setArtistModel(*myArtistModel);
    myUi->setAlbumModel(*myAlbumModel);
    myUi->setTrackModel(*myTrackModel);

    myFiltering = unique_ptr<Filtering>{new Filtering{*myUi, *myArtistRepository, *myAlbumRepository,
      *myTrackRepository, *myIndices}};
    myDataLoader = unique_ptr<DataLoader>{new DataLoader{myArtistRepository.get(), myAlbumRepository.get(),
        myTrackRepository.get(), *myAmpache, *myCache}};

    myUi->playTriggered += DELEGATE1(&AmpacheBrowserApp::onPlayTriggered, SelectedItems);
    myUi->createPlaylistTriggered += DELEGATE1(&AmpacheBrowserApp::onCreatePlaylistTriggered, SelectedItems);
    myUi->addToPlaylistTriggered += DELEGATE1(&AmpacheBrowserApp::onAddToPlaylistTriggered, SelectedItems);

    myUi->settingsUpdated += DELEGATE1(&AmpacheBrowserApp::onSettingsUpdated, tuple<bool, string, string, string>);

    myDataLoader->finished += DELEGATE1(&AmpacheBrowserApp::onDataLoaderFinished, LoadingResult);
}



void AmpacheBrowserApp::uninitializeDependencies() {
    // no need to unsubscribe from myDataLoader->finished event because the instance is destroyed below anyway; in fact
    // we can not unsubscribe because uninitializeDependencies() is called (also) from within the handler of the event;
    // in that case the unsubscription is postponed and it is called later when myDataLoaded instance is already gone

    myUi->settingsUpdated -= DELEGATE1(&AmpacheBrowserApp::onSettingsUpdated, tuple<bool, string, string, string>);

    myUi->addToPlaylistTriggered -= DELEGATE1(&AmpacheBrowserApp::onAddToPlaylistTriggered, SelectedItems);
    myUi->createPlaylistTriggered -= DELEGATE1(&AmpacheBrowserApp::onCreatePlaylistTriggered, SelectedItems);
    myUi->playTriggered -= DELEGATE1(&AmpacheBrowserApp::onPlayTriggered, SelectedItems);

    myDataLoader = nullptr;
    myFiltering = nullptr;

    myTrackModel = nullptr;
    myAlbumModel = nullptr;
    myArtistModel = nullptr;

    myTrackRepository = nullptr;
    myAlbumRepository = nullptr;
    myArtistRepository = nullptr;
}



void AmpacheBrowserApp::applySettings() {
    // SMELL: Handshake with the server can be in progress.  Unsubscribing from event here is most likely not enough.
    myAmpache->readySession -= DELEGATE1(&AmpacheBrowserApp::onPlayTriggeredAmpacheReadySession, bool);

    myDataLoader->aborted += DELEGATE0(&AmpacheBrowserApp::onApplySettingsDataLoaderAborted);
    myDataLoader->abort();
}



vector<string> AmpacheBrowserApp::createPlaylistItems(bool error) {
    if (error) {
        myUi->showNotification(_("Unable to connect to server."));
        // continue anyway
    }

    auto selectedTracks = myPlayIds.getTracks();

    // if no track selected, take all
    if (selectedTracks.size() == 0 && (myPlayIds.getArtists().size() != 0 || myPlayIds.getAlbums().size() != 0)) {
        for (int row = 0; row < myTrackModel->rowCount(); ++row) {
            selectedTracks.push_back(myTrackModel->data(myTrackModel->index(row, 3)).toString().toStdString());
        }
    }

    vector<string> playlistUrls;
    for (auto& id: selectedTracks) {
        auto trackUrl = myAmpache->refreshUrl(myTrackRepository->getById(id).getUrl());
        playlistUrls.push_back(trackUrl);
    }

    myPlayIds = SelectedItems{};
    return playlistUrls;
}

}
