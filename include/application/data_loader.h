// ampache_loader.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef DATALOADER_H
#define DATALOADER_H



#include "infrastructure/event/event.h"
#include "data/provider_type.h"

namespace data {
class ArtistRepository;
class AlbumRepository;
class TrackRepository;
class Ampache;
class Cache;
}



namespace application {

enum class LoadingResult {
    Success,
    SuccessNoConnection,
    NoConnectionNoCache,
    Error,
    Aborted
};



/**
 * @brief Coordinades data loading.
 */
class DataLoader {

public:
    explicit DataLoader(data::ArtistRepository* const artistRepository, data::AlbumRepository* const albumRepository,
        data::TrackRepository* const trackRepository, data::Ampache& ampache, data::Cache& cache);

    infrastructure::Event<LoadingResult> finished{};

    void load();

    void abort();

private:
    enum State {
        Idle,
        Loading,
        Aborting
    };

    // arguments from the constructor
    data::ArtistRepository* const myArtistRepository = nullptr;
    data::AlbumRepository* const myAlbumRepository = nullptr;
    data::TrackRepository* const myTrackRepository = nullptr;
    data::Ampache& myAmpache;
    data::Cache& myCache;

    State myState = Idle;

    // true if all all repositories are fully loaded (this does not include album arts)
    bool myDataLoaded = false;

    // true if album arts are fully loaded
    bool myAlbumArtsLoaded = false;

    data::ProviderType myProviderType = data::ProviderType::None;

    void onAmpacheInitialized(bool error);

    void onArtistsFullyLoaded(bool error);
    void onAlbumsFullyLoaded(bool error);
    void onTracksFullyLoaded(bool error);
    void onArtsFullyLoaded(bool error);

    void possiblyFireFinished();
    void fireFinished(LoadingResult loadingResult);

};

}



#endif // DATALOADER_H
