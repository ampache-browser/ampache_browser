// data_loader.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <chrono>

#include "infrastructure/event/delegate.h"
#include "infrastructure/logging/logging.h"
#include "data/provider_type.h"
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



bool DataLoader::isLoadingInProgress() const {
    return myState == Loading;
}



void DataLoader::load() {
    if (myState != Idle) {
        return;
    }

    LOG_INF("Begin loading.");
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
        fireAborted();
        return;
    }

    LOG_INF("Begin aborting.");
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
    LOG_INF("Ampache initialized with result %d.", error);
    myAmpache.initialized -= DELEGATE1(&DataLoader::onAmpacheInitialized, bool);
    myAmpacheInitializationFinished = true;

    myIsConnectionSuccessful = !error;
    if (myState == Aborting) {
        possiblyFireFinishedOrAborted();
        return;
    }

    // finish with error if neither Ampache nor cache is available
    if (error && (myCache.getServerUrl() != myAmpache.getUrl() || myCache.getUser() != myAmpache.getUser() ||
            myCache.getLastUpdate() == system_clock::time_point::min())) {
        fireFinished(LoadingResult::NoConnectionNoCache);
        return;
    }

    if (error || (myCache.getServerUrl() == myAmpache.getUrl() && myCache.getUser() == myAmpache.getUser() &&
            myCache.getLastUpdate() > myAmpache.getLastUpdate())) {
        LOG_DBG("Setting data provider type to Cache (artists: %d, albums: %d, tracks: %d).",
            myCache.numberOfArtists(), myCache.numberOfAlbums(), myCache.numberOfTracks());
        myProviderType = ProviderType::Cache;
    } else {
        LOG_DBG("Setting data provider type to Ampache (artists: %d, albums: %d, tracks: %d).",
            myAmpache.numberOfArtists(), myAmpache.numberOfAlbums(), myAmpache.numberOfTracks());
        myProviderType = ProviderType::Ampache;
    }

    myArtistRepository->fullyLoaded += DELEGATE1(&DataLoader::onArtistsFullyLoaded, bool);
    myArtistRepository->setProviderType(myProviderType);
}



void DataLoader::onArtistsFullyLoaded(bool error) {
    LOG_INF("Artists fully loaded with result %d.", error);
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
    LOG_INF("Albums fully loaded with result %d.", error);
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
    LOG_INF("Tracks fully loaded with result %d.", error);
    myTrackRepository->fullyLoaded -= DELEGATE1(&DataLoader::onTracksFullyLoaded, bool);
    myTracksLoadingFinished = true;
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    possiblyFireFinishedOrAborted();
}



void DataLoader::onArtsFullyLoaded(bool error) {
    LOG_INF("Arts fully loaded with result %d.", error);
    myAlbumRepository->artsFullyLoaded -= DELEGATE1(&DataLoader::onArtsFullyLoaded, bool);
    myAlbumArtsLoadingFinished = true;
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    possiblyFireFinishedOrAborted();
}



void DataLoader::onArtistRepositoryLoadingDisabled() {
    LOG_INF("Artists loading disabled.");
    myArtistsLoadingFinished = true;
    possiblyFireFinishedOrAborted();
}



void DataLoader::onAlbumRepositoryLoadingDisabled() {
    LOG_INF("Albums loading disabled.");
    myAlbumsLoadingFinished = true;
    possiblyFireFinishedOrAborted();
}



void DataLoader::onAlbumRepositoryArtsLoadingDisabled() {
    LOG_INF("Album arts loading disabled.");
    myAlbumArtsLoadingFinished = true;
    possiblyFireFinishedOrAborted();
}



void DataLoader::onTrackRepositoryLoadingDisabled() {
    LOG_INF("Tracks loading disabled.");
    myTracksLoadingFinished = true;
    possiblyFireFinishedOrAborted();
}



void DataLoader::possiblyFireFinishedOrAborted() {
    if (myAmpacheInitializationFinished && myArtistsLoadingFinished && myAlbumsLoadingFinished &&
        myAlbumArtsLoadingFinished && myTracksLoadingFinished) {
        if (myState == Aborting) {
            myTrackRepository->loadingDisabled -= DELEGATE0(&DataLoader::onTrackRepositoryLoadingDisabled);
            myAlbumRepository->artsLoadingDisabled -= DELEGATE0(&DataLoader::onAlbumRepositoryArtsLoadingDisabled);
            myAlbumRepository->loadingDisabled -= DELEGATE0(&DataLoader::onAlbumRepositoryLoadingDisabled);
            myArtistRepository->loadingDisabled -= DELEGATE0(&DataLoader::onArtistRepositoryLoadingDisabled);
            fireAborted();
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
    if (myState == Idle) {
        return;
    }

    LOG_INF("Data loader finished with result %d.", loadingResult);
    myState = Idle;
    finished(loadingResult);
}



void DataLoader::fireAborted() {
    LOG_INF("Data loader aborted.");
    myState = Idle;
    aborted();
}

}
