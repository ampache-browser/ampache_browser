// ampache_browser.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <fstream>
#include <memory>

#include "infrastructure/event/delegate.h"
#include "ui/ui.h"
#include "domain/artist.h"
#include "application/models/album_model.h"
#include "application/models/artist_model.h"
#include "application/models/track_model.h"
#include "data/providers/ampache_service.h"
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
    string url;
    string user;
    string pass;
    ifstream creds{"../../elab/creds"};
    if (creds.is_open()) {
        getline(creds, url);
        getline(creds, user);
        getline(creds, pass);
        creds.close();
    }
    myAmpacheService = unique_ptr<AmpacheService>{new AmpacheService{url, user, pass}};
    myAmpacheService->connected += DELEGATE0(&AmpacheBrowser::onConnected);
    myCache = unique_ptr<Cache>{new Cache{}};
}



void AmpacheBrowser::onConnected() {
    myAmpacheService->connected -= DELEGATE0(&AmpacheBrowser::onConnected);

    myIndices = unique_ptr<Indices>{new Indices{}};

    myArtistRepository = unique_ptr<ArtistRepository>{new ArtistRepository{*myAmpacheService, *myCache}};
    myAlbumRepository = unique_ptr<AlbumRepository>{new AlbumRepository{*myAmpacheService, *myCache,
      *myArtistRepository}};
    myTrackRepository = unique_ptr<TrackRepository>{new TrackRepository{*myAmpacheService, *myCache,
      *myArtistRepository, *myAlbumRepository, *myIndices}};

    myArtistRepository->fullyLoaded += DELEGATE0(&AmpacheBrowser::onArtistsFullyLoaded);
    myArtistModel = unique_ptr<ArtistModel>{new ArtistModel(*myArtistRepository)};
    myUi->setArtistModel(*myArtistModel);

    myUi->artistsSelected += DELEGATE1(&AmpacheBrowser::onArtistsSelected, vector<string>);
    myUi->albumsSelected += DELEGATE1(&AmpacheBrowser::onAlbumsSelected, vector<string>);
}



void AmpacheBrowser::onArtistsFullyLoaded() {
    myArtistRepository->fullyLoaded -= DELEGATE0(&AmpacheBrowser::onArtistsFullyLoaded);
    myAlbumRepository->fullyLoaded += DELEGATE0(&AmpacheBrowser::onAlbumsFullyLoaded);
    myAlbumModel = unique_ptr<AlbumModel>{new AlbumModel(*myAlbumRepository)};
    myUi->setAlbumModel(*myAlbumModel);
}



void AmpacheBrowser::onAlbumsFullyLoaded() {
    myAlbumRepository->fullyLoaded -= DELEGATE0(&AmpacheBrowser::onAlbumsFullyLoaded);
    myTrackRepository->fullyLoaded += DELEGATE0(&AmpacheBrowser::onTracksFullyLoaded);
    myTrackModel = unique_ptr<TrackModel>{new TrackModel(*myTrackRepository)};
    myUi->setTrackModel(*myTrackModel);
}



void AmpacheBrowser::onTracksFullyLoaded() {
    myTrackRepository->fullyLoaded -= DELEGATE0(&AmpacheBrowser::onTracksFullyLoaded);
    myUi->searchTriggered += DELEGATE1(&AmpacheBrowser::onSearchTriggered, string);
}



void AmpacheBrowser::onArtistsSelected(vector<string> ids) {
    if (ids.empty()) {
        myAlbumRepository->unsetFilter();
        myTrackRepository->unsetFilter();
    } else {
        vector<reference_wrapper<const Artist>> artists;
        for (auto id: ids) {
            auto& artist = myArtistRepository->getById(id);
            artists.push_back(artist);
        }
        myAlbumRepository->setFilter(unique_ptr<Filter<AlbumData>>{new ArtistFilterForAlbums{artists, *myIndices}});
        myTrackRepository->setFilter(unique_ptr<Filter<TrackData>>{new ArtistFilterForTracks{artists, *myIndices}});
    }
}



void AmpacheBrowser::onAlbumsSelected(vector<string> ids) {
    if (ids.empty()) {
        myTrackRepository->unsetFilter();
    } else {
        vector<reference_wrapper<const Album>> albums;
        for (auto id: ids) {
            auto& album = myAlbumRepository->getById(id);
            albums.push_back(album);
        }
        myTrackRepository->setFilter(unique_ptr<Filter<TrackData>>{new AlbumFilterForTracks{albums, *myIndices}});
    }
}



void AmpacheBrowser::onSearchTriggered(string searchText) {
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

}
