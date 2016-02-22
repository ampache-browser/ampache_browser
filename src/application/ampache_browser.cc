// ampache_browser.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <fstream>
#include <memory>

#include <libaudcore/runtime.h>
#include <libaudcore/index.h>
#include <libaudcore/tuple.h>
#include <libaudcore/playlist.h>

#include "infrastructure/event/delegate.h"
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
#include "data/album_repository.h"
#include "data/artist_repository.h"
#include "data/track_repository.h"
#include "data/indices.h"
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
    myAmpache->connected += DELEGATE0(&AmpacheBrowser::onConnected);

    myCache = unique_ptr<Cache>{new Cache{}};

    myIndices = unique_ptr<Indices>{new Indices{}};

    myArtistRepository = unique_ptr<ArtistRepository>{new ArtistRepository{*myAmpache, *myCache, *myIndices}};
    myAlbumRepository = unique_ptr<AlbumRepository>{new AlbumRepository{*myAmpache, *myCache,
      *myArtistRepository, *myIndices}};
    myTrackRepository = unique_ptr<TrackRepository>{new TrackRepository{*myAmpache, *myCache,
      *myArtistRepository, *myAlbumRepository, *myIndices}};

    myArtistModel = unique_ptr<ArtistModel>{new ArtistModel(*myArtistRepository)};
    myAlbumModel = unique_ptr<AlbumModel>{new AlbumModel(*myAlbumRepository)};
    myTrackModel = unique_ptr<TrackModel>{new TrackModel(*myTrackRepository)};

    myUi->artistsSelected += DELEGATE1(&AmpacheBrowser::onArtistsSelected, vector<string>);
    myUi->albumsSelected += DELEGATE1(&AmpacheBrowser::onAlbumsSelected, vector<string>);
    myUi->searchTriggered += DELEGATE1(&AmpacheBrowser::onSearchTriggered, string);
    myUi->playTriggered += DELEGATE1(&AmpacheBrowser::onPlayTriggered, vector<string>);
}



AmpacheBrowser::~AmpacheBrowser() {
    myUi->playTriggered -= DELEGATE1(&AmpacheBrowser::onPlayTriggered, vector<string>);
    myUi->searchTriggered -= DELEGATE1(&AmpacheBrowser::onSearchTriggered, string);
    myUi->albumsSelected -= DELEGATE1(&AmpacheBrowser::onAlbumsSelected, vector<string>);
    myUi->artistsSelected -= DELEGATE1(&AmpacheBrowser::onArtistsSelected, vector<string>);
}



void AmpacheBrowser::requestTermination() {
    myAmpache->connected -= DELEGATE0(&AmpacheBrowser::onConnected);
    myArtistRepository->fullyLoaded -= DELEGATE0(&AmpacheBrowser::onArtistsFullyLoaded);
    myAlbumRepository->fullyLoaded -= DELEGATE0(&AmpacheBrowser::onAlbumsFullyLoaded);
    myTrackRepository->fullyLoaded -= DELEGATE0(&AmpacheBrowser::onTracksFullyLoaded);

    myArtistModel->dataRequestsAborted += DELEGATE0(&AmpacheBrowser::onArtistDataRequestsAborted);
    myAlbumModel->dataRequestsAborted += DELEGATE0(&AmpacheBrowser::onAlbumDataRequestsAborted);
    myTrackModel->dataRequestsAborted += DELEGATE0(&AmpacheBrowser::onTrackDataRequestsAborted);
    myArtistModel->abortDataRequests();
    myAlbumModel->abortDataRequests();
    myTrackModel->abortDataRequests();
}



void AmpacheBrowser::onConnected() {
    myAmpache->connected -= DELEGATE0(&AmpacheBrowser::onConnected);
    myArtistRepository->fullyLoaded += DELEGATE0(&AmpacheBrowser::onArtistsFullyLoaded);
    myArtistModel->requestAllData();
    myUi->setArtistModel(*myArtistModel);
}



void AmpacheBrowser::onArtistsFullyLoaded() {
    myArtistRepository->fullyLoaded -= DELEGATE0(&AmpacheBrowser::onArtistsFullyLoaded);
    myAlbumRepository->fullyLoaded += DELEGATE0(&AmpacheBrowser::onAlbumsFullyLoaded);
    myAlbumModel->requestAllData();
    myUi->setAlbumModel(*myAlbumModel);
}



void AmpacheBrowser::onAlbumsFullyLoaded() {
    myAlbumRepository->fullyLoaded -= DELEGATE0(&AmpacheBrowser::onAlbumsFullyLoaded);
    myTrackRepository->fullyLoaded += DELEGATE0(&AmpacheBrowser::onTracksFullyLoaded);
    myTrackModel->requestAllData();
    myUi->setTrackModel(*myTrackModel);
}



void AmpacheBrowser::onTracksFullyLoaded() {
    myTrackRepository->fullyLoaded -= DELEGATE0(&AmpacheBrowser::onTracksFullyLoaded);
}



void AmpacheBrowser::onPlayTriggered(const vector<string>& ids) {
    auto actualIds = ids;
    
    // if nothing selected, take all
    if (actualIds.size() == 0) {
        for (int row = 0; row < myTrackModel->rowCount(); ++row) {
            actualIds.push_back(myTrackModel->data(myTrackModel->index(row, 3)).toString().toStdString());
        }
    }

    Index<PlaylistAddItem> playlistAddItems;
    for (auto& id: actualIds) {
        Tuple tuple;
        auto trackUrl = myAmpache->refreshUrl(myTrackRepository->getById(id).getUrl());
        playlistAddItems.append(String{trackUrl.c_str()}, move(tuple), nullptr);
    }
    auto activePlaylist = aud_playlist_get_active();
    aud_playlist_entry_insert_batch(activePlaylist, -1, move(playlistAddItems), true);
}



void AmpacheBrowser::onArtistsSelected(const vector<string>& ids) {
    if (ids.empty()) {
        myAlbumRepository->unsetFilter();
        myTrackRepository->unsetFilter();
    } else {
        vector<reference_wrapper<const Artist>> artists;
        for (auto& id: ids) {
            auto& artist = myArtistRepository->getById(id);
            artists.push_back(artist);
        }
        myAlbumRepository->setFilter(unique_ptr<Filter<AlbumData>>{new ArtistFilterForAlbums{artists, *myIndices}});
        myTrackRepository->setFilter(unique_ptr<Filter<TrackData>>{new ArtistFilterForTracks{artists, *myIndices}});
    }
}



void AmpacheBrowser::onAlbumsSelected(const vector<string>& ids) {
    if (ids.empty()) {
        myTrackRepository->unsetFilter();
    } else {
        vector<reference_wrapper<const Album>> albums;
        for (auto& id: ids) {
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



void AmpacheBrowser::onArtistDataRequestsAborted() {
    myIsArtistDataRequestAborted = true;
    possiblyRaiseTerminated();
}



void AmpacheBrowser::onAlbumDataRequestsAborted() {
    myIsAlbumDataRequestAborted = true;
    possiblyRaiseTerminated();
}



void AmpacheBrowser::onTrackDataRequestsAborted() {
    myIsTrackDataRequestAborted = true;
    possiblyRaiseTerminated();
}



void AmpacheBrowser::possiblyRaiseTerminated() {
    if (myIsArtistDataRequestAborted && myIsAlbumDataRequestAborted && myIsTrackDataRequestAborted) {
        myArtistModel->dataRequestsAborted -= DELEGATE0(&AmpacheBrowser::onArtistDataRequestsAborted);
        myAlbumModel->dataRequestsAborted -= DELEGATE0(&AmpacheBrowser::onAlbumDataRequestsAborted);
        myTrackModel->dataRequestsAborted -= DELEGATE0(&AmpacheBrowser::onTrackDataRequestsAborted);
        terminated();
    }
}

}
