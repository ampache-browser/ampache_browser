// ampache_browser.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <fstream>
#include <memory>

#include <libaudcore/runtime.h>
#include <libaudcore/i18n.h>
#include <libaudcore/index.h>
#include <libaudcore/tuple.h>
#include <libaudcore/playlist.h>

#include "infrastructure/event/delegate.h"
#include "ui/selected_items.h"
#include "ui/ui.h"
#include "domain/artist.h"
#include "application/models/album_model.h"
#include "application/models/artist_model.h"
#include "application/models/track_model.h"
#include "data/providers/ampache.h"
#include "data/providers/cache.h"
#include "data/filters/name_filter_for_artists.h"
#include "data/filters/artist_filter_for_albums.h"
#include "data/filters/name_filter_for_albums.h"
#include "data/filters/artist_filter_for_tracks.h"
#include "data/filters/album_filter_for_tracks.h"
#include "data/filters/name_filter_for_tracks.h"
#include "data/repositories/album_repository.h"
#include "data/repositories/artist_repository.h"
#include "data/repositories/track_repository.h"
#include "data/indices.h"
#include "application/data_loader.h"
#include "application/ampache_browser.h"

using namespace std;
using namespace placeholders;
using namespace infrastructure;
using namespace domain;
using namespace data;
using namespace ui;



namespace application {

AmpacheBrowser::AmpacheBrowser(Ui& ui):
myUi(&ui) {
    auto url = string{aud_get_str("ampache_browser", "url")};
    auto username = string{aud_get_str("ampache_browser", "username")};
    auto passwordHash = string{aud_get_str("ampache_browser", "password_hash")};

    myAmpache = unique_ptr<Ampache>{new Ampache{url, username, passwordHash}};
    myCache = unique_ptr<Cache>{new Cache{url, username}};
    myIndices = unique_ptr<Indices>{new Indices{}};

    myArtistRepository = unique_ptr<ArtistRepository>{new ArtistRepository{*myAmpache, *myCache, *myIndices}};
    myAlbumRepository = unique_ptr<AlbumRepository>{new AlbumRepository{*myAmpache, *myCache, *myIndices,
        myArtistRepository.get()}};
    myTrackRepository = unique_ptr<TrackRepository>{new TrackRepository{*myAmpache, *myCache, *myIndices,
        myArtistRepository.get(), myAlbumRepository.get()}};

    myArtistModel = unique_ptr<ArtistModel>{new ArtistModel(myArtistRepository.get())};
    myAlbumModel = unique_ptr<AlbumModel>{new AlbumModel(myAlbumRepository.get())};
    myTrackModel = unique_ptr<TrackModel>{new TrackModel(myTrackRepository.get())};

    myUi->artistsSelected += DELEGATE1(&AmpacheBrowser::onArtistsSelected, vector<string>);
    myUi->albumsSelected += DELEGATE1(&AmpacheBrowser::onAlbumsSelected, pair<vector<string>, vector<string>>);
    myUi->searchTriggered += DELEGATE1(&AmpacheBrowser::onSearchTriggered, string);
    myUi->playTriggered += DELEGATE1(&AmpacheBrowser::onPlayTriggered, SelectedItems);
    myUi->createPlaylistTriggered += DELEGATE1(&AmpacheBrowser::onCreatePlaylistTriggered, SelectedItems);
    myUi->addToPlaylistTriggered += DELEGATE1(&AmpacheBrowser::onAddToPlaylistTriggered, SelectedItems);

    myUi->setArtistModel(*myArtistModel);
    myUi->setAlbumModel(*myAlbumModel);
    myUi->setTrackModel(*myTrackModel);

    myDataLoader = unique_ptr<DataLoader>{new DataLoader{myArtistRepository.get(), myAlbumRepository.get(),
        myTrackRepository.get(), *myAmpache, *myCache}};
    myDataLoader->finished += DELEGATE1(&AmpacheBrowser::onDataLoaderFinished, LoadingResult);
    myDataLoader->load();
}



AmpacheBrowser::~AmpacheBrowser() {
    myDataLoader->finished -= DELEGATE1(&AmpacheBrowser::onDataLoaderFinished, LoadingResult);

    myUi->addToPlaylistTriggered -= DELEGATE1(&AmpacheBrowser::onAddToPlaylistTriggered, SelectedItems);
    myUi->createPlaylistTriggered -= DELEGATE1(&AmpacheBrowser::onCreatePlaylistTriggered, SelectedItems);
    myUi->playTriggered -= DELEGATE1(&AmpacheBrowser::onPlayTriggered, SelectedItems);
    myUi->searchTriggered -= DELEGATE1(&AmpacheBrowser::onSearchTriggered, string);
    myUi->albumsSelected -= DELEGATE1(&AmpacheBrowser::onAlbumsSelected, pair<vector<string>, vector<string>>);
    myUi->artistsSelected -= DELEGATE1(&AmpacheBrowser::onArtistsSelected, vector<string>);
}



void AmpacheBrowser::requestTermination() {
    AUDINFO("Termination request.\n");

    // SMELL: Handshake with the server can be in progress.  Unsubscribing from event here is most likely not enough.
    myAmpache->readySession -= DELEGATE1(&AmpacheBrowser::onPlayOrCreateReadySession, bool);

    myDataLoader->abort();
}



void AmpacheBrowser::onDataLoaderFinished(LoadingResult loadingResult) {
    switch (loadingResult) {
        case LoadingResult::Error:
            myUi->showNotification(_("Error while reading data!"));
            break;
        case LoadingResult::NoConnectionNoCache:
        case LoadingResult::SuccessNoConnection:
            myUi->showNotification(_("Unable to connect to server."));
            break;
        case LoadingResult::Aborted:
            terminated();
            break;
        default:
            myUi->showNotification(_("Loaded."));
    }
}



void AmpacheBrowser::onPlayTriggered(SelectedItems& selectedItems) {
    myPlayIds = move(selectedItems);
    myAmpache->readySession += DELEGATE1(&AmpacheBrowser::onPlayOrCreateReadySession, bool);
    myAmpache->refreshSession();
}



void AmpacheBrowser::onCreatePlaylistTriggered(SelectedItems& selectedItems) {
    aud_playlist_new();

    myPlayIds = move(selectedItems);
    myAmpache->readySession += DELEGATE1(&AmpacheBrowser::onPlayOrCreateReadySession, bool);
    myAmpache->refreshSession();
}



void AmpacheBrowser::onAddToPlaylistTriggered(SelectedItems& selectedItems) {
    myPlayIds = move(selectedItems);
    myAmpache->readySession += DELEGATE1(&AmpacheBrowser::onAddReadySession, bool);
    myAmpache->refreshSession();
}



void AmpacheBrowser::onArtistsSelected(const vector<string>& ids) {
    if (ids.empty()) {
        myAlbumRepository->unsetFilter();
        myTrackRepository->unsetFilter();
    } else {
        setArtistFilters(ids);
    }
}



void AmpacheBrowser::onAlbumsSelected(const pair<vector<string>, vector<string>>& albumAndArtistIds) {
    if (albumAndArtistIds.first.empty()) {
        if (albumAndArtistIds.second.empty()) {
            myTrackRepository->unsetFilter();
        } else {
            setArtistFilters(albumAndArtistIds.second);
        }
    } else {
        vector<reference_wrapper<const Album>> albums;
        for (auto& id: albumAndArtistIds.first) {
            auto& album = myAlbumRepository->getById(id);
            albums.push_back(album);
        }
        myTrackRepository->setFilter(unique_ptr<Filter<TrackData>>{new AlbumFilterForTracks{albums, *myIndices}});
    }
}



void AmpacheBrowser::onSearchTriggered(const string& searchText) {
    if (searchText.empty()) {
        myArtistRepository->unsetFilter();
        myAlbumRepository->unsetFilter();
        myTrackRepository->unsetFilter();
    } else {
        myArtistRepository->setFilter(unique_ptr<Filter<ArtistData>>{new NameFilterForArtists{searchText}});
        myAlbumRepository->setFilter(unique_ptr<Filter<AlbumData>>{new NameFilterForAlbums{searchText}});
        myTrackRepository->setFilter(unique_ptr<Filter<TrackData>>{new NameFilterForTracks{searchText}});
    }
}



void AmpacheBrowser::onPlayOrCreateReadySession(bool error) {
    myAmpache->readySession -= DELEGATE1(&AmpacheBrowser::onPlayOrCreateReadySession, bool);

    auto playlistItems = createPlaylistItems(error);
    auto activePlaylist = aud_playlist_get_active();
    aud_playlist_entry_insert_batch(activePlaylist, -1, move(playlistItems), true);
}



void AmpacheBrowser::onAddReadySession(bool error) {
    myAmpache->readySession -= DELEGATE1(&AmpacheBrowser::onAddReadySession, bool);

    auto playlistItems = createPlaylistItems(error);
    auto activePlaylist = aud_playlist_get_active();
    aud_playlist_entry_insert_batch(activePlaylist, -1, move(playlistItems), false);
}



Index<PlaylistAddItem> AmpacheBrowser::createPlaylistItems(bool error) {
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

    Index<PlaylistAddItem> playlistAddItems;
    for (auto& id: selectedTracks) {
        auto trackUrl = myAmpache->refreshUrl(myTrackRepository->getById(id).getUrl());
        Tuple tuple;
        playlistAddItems.append(String{trackUrl.c_str()}, move(tuple), nullptr);
    }

    myPlayIds = SelectedItems{};
    return playlistAddItems;
}



void AmpacheBrowser::setArtistFilters(const vector<string>& ids) {
    vector<reference_wrapper<const Artist>> artists;
    for (auto& id: ids) {
        auto& artist = myArtistRepository->getById(id);
        artists.push_back(artist);
    }
    myAlbumRepository->setFilter(unique_ptr<Filter<AlbumData>>{new ArtistFilterForAlbums{artists, *myIndices}});
    myTrackRepository->setFilter(unique_ptr<Filter<TrackData>>{new ArtistFilterForTracks{artists, *myIndices}});
}

}
