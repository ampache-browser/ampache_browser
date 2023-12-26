// artist_repository.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2023 Róbert Čerňanský



#ifndef ARTISTREPOSITORY_H
#define ARTISTREPOSITORY_H



#include "src/data/data_objects/artist_data.h"
#include "domain/artist.h"
#include "data/repositories/repository.h"



namespace data {

class Ampache;
class Cache;
class Indices;



/**
 * @brief Stores artists data, provides means to trigger their load from Ampache server or a cache; updates indices.
 */
class ArtistRepository: public Repository<ArtistData, domain::Artist> {

public:
    /**
     * @brief Constructor.
     *
     * @param ampache Used for communication with Ampache server.
     * @param cache Used for accessing the disk cache.
     * @param indices Indices to update.
     */
    explicit ArtistRepository(Ampache& ampache, Cache& cache, Indices& indices);

    ArtistRepository(const ArtistRepository& other) = delete;

    ArtistRepository& operator=(const ArtistRepository& other) = delete;

    int dataProviderCount() const override;

protected:
    void requestDataLoad(int offset, int limit) override;

    domain::Artist& getDomainObject(const ArtistData& dataItem) const override;

    infrastructure::Event<std::pair<std::vector<std::unique_ptr<ArtistData>>, bool>>& getDataLoadRequestFinishedEvent() override;

    void loadDataFromCache() override;

    void saveDataToCache() override;

    void updateIndices(const std::vector<std::unique_ptr<ArtistData>>& data) override;

    void clearIndices() override;
};

}

#endif // ARTISTREPOSITORY_H
