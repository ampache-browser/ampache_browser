// data_loader.h
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

/**
 * @brief Data loading result.
 */
enum class LoadingResult {
    /**
     * @brief Loading was successful.
     */
    Success,

    /**
     * @brief Connection to the server failed however data were loaded from the cache.
     */
    SuccessNoConnection,

    /**
     * @brief Conection to the server failed and cached data were not available.
     *
     */
    NoConnectionNoCache,

    /**
     * @brief Error occured while readind data.
     */
    Error,

    /**
     * @brief Loading was aborted.
     */
    Aborted
};



/**
 * @brief Coordinades data loading.
 */
class DataLoader {

public:
    explicit DataLoader(data::ArtistRepository* const artistRepository, data::AlbumRepository* const albumRepository,
        data::TrackRepository* const trackRepository, data::Ampache& ampache, data::Cache& cache);

    /**
     * @brief Fired when loading is finished.
     */
    infrastructure::Event<LoadingResult> finished{};

    /**
     * @brief Returns true if loading is in progress.
     */
    bool isLoadingInProgress() const;

    /**
     * @brief Load data.
     *
     * @note Previously loaded data are cleared.
     */
    void load();

    /**
     * @brief Abort loading.
     */
    void abort();

private:
    // loading state
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

    // current loading state
    State myState = Idle;

    // true if handshake with the server was successful
    bool myIsConnectionSuccessful = false;

    // true if initialization or loading of particular data has finished
    bool myAmpacheInitializationFinished = false;
    bool myArtistsLoadingFinished = false;
    bool myAlbumsLoadingFinished = false;
    bool myAlbumArtsLoadingFinished = false;
    bool myTracksLoadingFinished = false;

    // data provider which is currently used
    data::ProviderType myProviderType = data::ProviderType::None;

    void onAmpacheInitialized(bool error);

    void onArtistsFullyLoaded(bool error);
    void onAlbumsFullyLoaded(bool error);
    void onTracksFullyLoaded(bool error);
    void onArtsFullyLoaded(bool error);

    void onArtistRepositoryLoadingDisabled();
    void onAlbumRepositoryLoadingDisabled();
    void onAlbumRepositoryArtsLoadingDisabled();
    void onTrackRepositoryLoadingDisabled();

    void possiblyFireFinished();
    void fireFinished(LoadingResult loadingResult);
};

}



#endif // DATALOADER_H
