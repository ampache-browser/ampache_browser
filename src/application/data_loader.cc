// ampache_loader.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <chrono>

#include <libaudcore/runtime.h>

#include "infrastructure/event/delegate.h"
#include "data/providers/ampache.h"
#include "data/providers/cache.h"
#include "data/repositories/artist_repository.h"
#include "data/repositories/album_repository.h"
#include "data/repositories/track_repository.h"
#include "application/data_loader.h"

using namespace std::chrono;
using namespace infrastructure;
using namespace data;



namespace application {

DataLoader::DataLoader(ArtistRepository* const artistRepository, AlbumRepository* const albumRepository,
TrackRepository* const trackRepository, Ampache& ampache, Cache& cache):
myArtistRepository(artistRepository),
myAlbumRepository(albumRepository),
myTrackRepository(trackRepository),
myAmpache(ampache),
myCache(cache) {
}



void DataLoader::load() {
    if (myState != Idle) {
        return;
    }

    AUDINFO("Begin loading.\n");
    myState = Loading;
    myIsConnectionSuccessful = false;

    myAmpacheInitializationFinished = false;
    myArtistsLoadingFinished = false;
    myAlbumsLoadingFinished = false;
    myAlbumArtsLoadingFinished = false;
    myTracksLoadingFinished = false;

    myTrackRepository->setProviderType(ProviderType::None);
    myAlbumRepository->setProviderType(ProviderType::None);
    myArtistRepository->setProviderType(ProviderType::None);

    myAmpache.initialized += DELEGATE1(&DataLoader::onAmpacheInitialized, bool);
    myAmpache.initialize();
}



void DataLoader::abort() {
    if (myState == Aborting) {
        return;
    }

    if (myState == Idle) {
        fireFinished(LoadingResult::Aborted);
        return;
    }

    AUDINFO("Begin aborting\n");
    myState = Aborting;

    myArtistRepository->loadingDisabled += DELEGATE0(&DataLoader::onArtistRepositoryLoadingDisabled);
    myAlbumRepository->loadingDisabled += DELEGATE0(&DataLoader::onAlbumRepositoryLoadingDisabled);
    myAlbumRepository->artsLoadingDisabled += DELEGATE0(&DataLoader::onAlbumRepositoryArtsLoadingDisabled);
    myTrackRepository->loadingDisabled += DELEGATE0(&DataLoader::onTrackRepositoryLoadingDisabled);

    myTrackRepository->disableLoading();
    myAlbumRepository->disableLoading();
    myArtistRepository->disableLoading();
}



void DataLoader::onAmpacheInitialized(bool error) {
    AUDINFO("Ampache initialized with result %d.\n", error);
    myAmpache.initialized -= DELEGATE1(&DataLoader::onAmpacheInitialized, bool);
    myAmpacheInitializationFinished = true;

    myIsConnectionSuccessful = !error;
    if (myState == Aborting) {
        possiblyFireFinished();
        return;
    }

    // finish with error if neither Ampache nor cache is available
    if (error && (myCache.getLastUpdate() == system_clock::time_point::min())) {
        fireFinished(LoadingResult::NoConnectionNoCache);
        return;
    }

    if (error || (myCache.getLastUpdate() > myAmpache.getLastUpdate())) {
        AUDDBG("Setting data provider type to Cache (artists: %d, albums: %d, tracks: %d).\n",
            myCache.numberOfArtists(), myCache.numberOfAlbums(), myCache.numberOfTracks());
        myProviderType = ProviderType::Cache;
    } else {
        AUDDBG("Setting data provider type to Ampache (artists: %d, albums: %d, tracks: %d).\n",
            myAmpache.numberOfArtists(), myAmpache.numberOfAlbums(), myAmpache.numberOfTracks());
        myProviderType = ProviderType::Ampache;
    }

    myArtistRepository->fullyLoaded += DELEGATE1(&DataLoader::onArtistsFullyLoaded, bool);
    myArtistRepository->setProviderType(myProviderType);
}



void DataLoader::onArtistsFullyLoaded(bool error) {
    AUDINFO("Artists fully loaded with result %d.\n", error);
    myArtistRepository->fullyLoaded -= DELEGATE1(&DataLoader::onArtistsFullyLoaded, bool);
    myArtistsLoadingFinished = true;
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    myAlbumRepository->fullyLoaded += DELEGATE1(&DataLoader::onAlbumsFullyLoaded, bool);
    myAlbumRepository->artsFullyLoaded += DELEGATE1(&DataLoader::onArtsFullyLoaded, bool);
    myAlbumRepository->setProviderType(myProviderType);
}



void DataLoader::onAlbumsFullyLoaded(bool error) {
    AUDINFO("Albums fully loaded with result %d.\n", error);
    myAlbumRepository->fullyLoaded -= DELEGATE1(&DataLoader::onAlbumsFullyLoaded, bool);
    myAlbumsLoadingFinished = true;
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    myTrackRepository->fullyLoaded += DELEGATE1(&DataLoader::onTracksFullyLoaded, bool);
    myTrackRepository->setProviderType(myProviderType);
}



void DataLoader::onTracksFullyLoaded(bool error) {
    AUDINFO("Tracks fully loaded with result %d.\n", error);
    myTrackRepository->fullyLoaded -= DELEGATE1(&DataLoader::onTracksFullyLoaded, bool);
    myTracksLoadingFinished = true;
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    possiblyFireFinished();
}



void DataLoader::onArtsFullyLoaded(bool error) {
    AUDINFO("Arts fully loaded with result %d.\n", error);
    myAlbumRepository->artsFullyLoaded -= DELEGATE1(&DataLoader::onArtsFullyLoaded, bool);
    myAlbumArtsLoadingFinished = true;
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    possiblyFireFinished();
}



void DataLoader::onArtistRepositoryLoadingDisabled() {
    AUDINFO("Artists loading disabled.\n");
    myArtistsLoadingFinished = true;
    possiblyFireFinished();
}



void DataLoader::onAlbumRepositoryLoadingDisabled() {
    AUDINFO("Albums loading disabled.\n");
    myAlbumsLoadingFinished = true;
    possiblyFireFinished();
}



void DataLoader::onAlbumRepositoryArtsLoadingDisabled() {
    AUDINFO("Album arts loading disabled.\n");
    myAlbumArtsLoadingFinished = true;
    possiblyFireFinished();
}



void DataLoader::onTrackRepositoryLoadingDisabled() {
    AUDINFO("Tracks loading disabled.\n");
    myTracksLoadingFinished = true;
    possiblyFireFinished();
}



void DataLoader::possiblyFireFinished() {
    if (myAmpacheInitializationFinished && myArtistsLoadingFinished && myAlbumsLoadingFinished &&
        myAlbumArtsLoadingFinished && myTracksLoadingFinished) {
        if (myState == Aborting) {
            myTrackRepository->loadingDisabled -= DELEGATE0(&DataLoader::onTrackRepositoryLoadingDisabled);
            myAlbumRepository->artsLoadingDisabled -= DELEGATE0(&DataLoader::onAlbumRepositoryArtsLoadingDisabled);
            myAlbumRepository->loadingDisabled -= DELEGATE0(&DataLoader::onAlbumRepositoryLoadingDisabled);
            myArtistRepository->loadingDisabled -= DELEGATE0(&DataLoader::onArtistRepositoryLoadingDisabled);
            fireFinished(LoadingResult::Aborted);
        } else {
            if (myIsConnectionSuccessful) {
                fireFinished(LoadingResult::Success);
            } else {
                fireFinished(LoadingResult::SuccessNoConnection);
            }
        }
    }
}



void DataLoader::fireFinished(LoadingResult loadingResult) {
    AUDINFO("Data loader finished with result %d.\n", loadingResult);
    myState = Idle;
    finished(loadingResult);
}

}
