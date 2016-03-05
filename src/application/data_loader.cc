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
#include "data/artist_repository.h"
#include "data/album_repository.h"
#include "data/track_repository.h"
#include "application/data_loader.h"

using namespace std::chrono;
using namespace infrastructure;
using namespace data;



namespace application {

DataLoader::DataLoader(ArtistRepository& artistRepository, AlbumRepository& albumRepository,
    TrackRepository& trackRepository, Ampache& ampache, Cache& cache):
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

    myDataLoaded = false;
    myAlbumArtsLoaded = false;

    myTrackRepository.setProviderType(ProviderType::None);
    myAlbumRepository.setProviderType(ProviderType::None);
    myArtistRepository.setProviderType(ProviderType::None);

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

    // disable loading on repositories so subsequent loading related events will be handled in regards of that
    // TODO: www Do to fire fullyLoaded upon disableLoading(); this class will handle loadingDisabled event; ready
    // methods in repositories will fire also loadingDisabled(); models will not handle loadingDisabled - let them
    // continue requesing without any effect; counts in repos will not be affected by disableLoading()
    myTrackRepository.disableLoading();
    myAlbumRepository.disableLoading();
    myArtistRepository.disableLoading();
}



// TODO: Start timer if Ampache was not available.  Or do it in AmpacheBrowser?
void DataLoader::onAmpacheInitialized(bool error) {
    AUDINFO("Ampache initialized with result %d.\n", error);

    // finish with error if neither Ampache nor cache is available
    if (error && (myCache.getLastUpdate() == system_clock::time_point::min())) {
        fireFinished(LoadingResult::Error);
        return;
    }

    myAmpache.initialized -= DELEGATE1(&DataLoader::onAmpacheInitialized, bool);

    if (error || (myCache.getLastUpdate() > myAmpache.getLastUpdate())) {
        AUDDBG("Setting data provider type to Cache (artists: %d, albums: %d, tracks: %d).\n",
            myCache.numberOfArtists(), myCache.numberOfAlbums(), myCache.numberOfTracks());
        myProviderType = ProviderType::Cache;
    } else {
        AUDDBG("Setting data provider type to Ampache (artists: %d, albums: %d, tracks: %d).\n",
            myAmpache.numberOfArtists(), myAmpache.numberOfAlbums(), myAmpache.numberOfTracks());
        myProviderType = ProviderType::Ampache;
    }

    myArtistRepository.fullyLoaded += DELEGATE1(&DataLoader::onArtistsFullyLoaded, bool);
    myArtistRepository.setProviderType(myProviderType);
}



void DataLoader::onArtistsFullyLoaded(bool error) {
    AUDINFO("Artists fully loaded with result %d.\n", error);
    myArtistRepository.fullyLoaded -= DELEGATE1(&DataLoader::onArtistsFullyLoaded, bool);
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    myAlbumRepository.fullyLoaded += DELEGATE1(&DataLoader::onAlbumsFullyLoaded, bool);
    myAlbumRepository.artsFullyLoaded += DELEGATE1(&DataLoader::onArtsFullyLoaded, bool);
    myAlbumRepository.setProviderType(myProviderType);
}



void DataLoader::onAlbumsFullyLoaded(bool error) {
    AUDINFO("Albums fully loaded with result %d.\n", error);
    myAlbumRepository.fullyLoaded -= DELEGATE1(&DataLoader::onAlbumsFullyLoaded, bool);
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    myTrackRepository.fullyLoaded += DELEGATE1(&DataLoader::onTracksFullyLoaded, bool);
    myTrackRepository.setProviderType(myProviderType);
}



void DataLoader::onTracksFullyLoaded(bool error) {
    AUDINFO("Tracks fully loaded with result %d.\n", error);
    myTrackRepository.fullyLoaded -= DELEGATE1(&DataLoader::onTracksFullyLoaded, bool);
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    myDataLoaded = true;
    possiblyFireFinished();
}



void DataLoader::onArtsFullyLoaded(bool error) {
    AUDINFO("Arts fully loaded with result %d.\n", error);
    myAlbumRepository.artsFullyLoaded -= DELEGATE1(&DataLoader::onArtsFullyLoaded, bool);
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    myAlbumArtsLoaded = true;
    possiblyFireFinished();
}



void DataLoader::possiblyFireFinished() {
    if (myDataLoaded && myAlbumArtsLoaded) {
        if (myState == Aborting) {
            fireFinished(LoadingResult::Aborted);
        } else {
            fireFinished(LoadingResult::Success);
        }
    }
}



void DataLoader::fireFinished(LoadingResult loadingResult) {
    AUDINFO("Data loading finished with result %d.\n", loadingResult);
    myState = Idle;
    finished(loadingResult);
}

}
