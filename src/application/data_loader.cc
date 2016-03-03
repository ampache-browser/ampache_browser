// ampache_loader.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <chrono>
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

    myState = Loading;

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

    myState = Aborting;

    // disable loading on repositories so subsequent loading related events will be handled in regards of that
    myTrackRepository.disableLoading();
    myAlbumRepository.disableLoading();
    myArtistRepository.disableLoading();
}



// TODO: Start timer if Ampache was not available.  Or do it in AmpacheBrowser?
void DataLoader::onAmpacheInitialized(bool error) {
    // finish with error if neither Ampache nor cache is available
    if (error && (myCache.getLastUpdate() == system_clock::time_point::min())) {
        fireFinished(LoadingResult::Error);
        return;
    }

    myAmpache.initialized -= DELEGATE1(&DataLoader::onAmpacheInitialized, bool);

    if (error || (myCache.getLastUpdate() > myAmpache.getLastUpdate())) {
        myProviderType = ProviderType::Cache;
    } else {
        myProviderType = ProviderType::Ampache;
    }

    myArtistRepository.fullyLoaded += DELEGATE1(&DataLoader::onArtistsFullyLoaded, bool);
    myArtistRepository.setProviderType(myProviderType);
}



void DataLoader::onArtistsFullyLoaded(bool error) {
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
    myAlbumRepository.fullyLoaded -= DELEGATE1(&DataLoader::onAlbumsFullyLoaded, bool);
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    myTrackRepository.fullyLoaded += DELEGATE1(&DataLoader::onTracksFullyLoaded, bool);
    myTrackRepository.setProviderType(myProviderType);
}



void DataLoader::onTracksFullyLoaded(bool error) {
    myTrackRepository.fullyLoaded -= DELEGATE1(&DataLoader::onTracksFullyLoaded, bool);
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }
}



void DataLoader::onArtsFullyLoaded(bool error) {
    myAlbumRepository.artsFullyLoaded -= DELEGATE1(&DataLoader::onArtsFullyLoaded, bool);
    if (error) {
        fireFinished(LoadingResult::Error);
        return;
    }

    if (myState == Aborting) {
        fireFinished(LoadingResult::Aborted);
    } else {
        fireFinished(LoadingResult::Success);
    }
}



void DataLoader::fireFinished(LoadingResult loadingResult) {
    myState = Idle;
    finished(loadingResult);
}

}
