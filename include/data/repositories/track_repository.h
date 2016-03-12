// track_repository.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef TRACKREPOSITORY_H
#define TRACKREPOSITORY_H



#include "src/data/data_objects/track_data.h"
#include "domain/track.h"
#include "data/repositories/repository.h"



namespace data {

class Ampache;
class Cache;
class ArtistRepository;
class AlbumRepository;
class Indices;



/**
 * @brief Stores tracks data, provides means to trigger their load from Ampache server or a cache; updates indices.
 *
 * @note This repository can be used only after AlbumRepository is fully loaded.
 */
class TrackRepository: public Repository<TrackData, domain::Track> {

public:
    /**
     * @brief Constructor.
     *
     * @param ampache Used for communication with Ampache server.
     * @param cache Used for accessing the disk cache.
     * @param artistRepository Used to set track artist.
     * @param albumRepository Used to set album of the track.
     * @param indices Indices to update.
     */
    explicit TrackRepository(Ampache& ampache, Cache& cache, Indices& indices,
        const ArtistRepository* const artistRepository, const AlbumRepository* const albumRepository);

    TrackRepository(const TrackRepository& other) = delete;

    TrackRepository& operator=(const TrackRepository& other) = delete;

protected:
    void requestDataLoad(int offset, int limit) override;

    domain::Track& getDomainObject(const TrackData& dataItem) const override;

    infrastructure::Event<std::vector<std::unique_ptr<TrackData>>>& getDataLoadRequestFinishedEvent() override;

    void loadDataFromCache() override;

    void saveDataToCache() override;

    void handleLoadedItem(const TrackData& dataItem) const override;

    void updateIndices(const std::vector<std::unique_ptr<TrackData>>& data) override;

    void clearIndices() override;

    int getMaxDataSize() const override;

private:
    // arguments from the constructor
    const ArtistRepository* const myArtistRepository = nullptr;
    const AlbumRepository* const myAlbumRepository = nullptr;
};

}

#endif // TRACKREPOSITORY_H
