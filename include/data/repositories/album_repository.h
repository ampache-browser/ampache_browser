// album_repository.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ALBUMREPOSITORY_H
#define ALBUMREPOSITORY_H



#include <string>
#include <vector>

#include "infrastructure/event/event.h"
#include "src/data/data_objects/album_data.h"
#include "domain/album.h"
#include "data/provider_type.h"
#include "data/repositories/repository.h"



namespace data {

class Ampache;
class Cache;
class ArtistRepository;
class Indices;



/**
 * @brief Stores albums data, provides means to trigger their load from Ampache server or a cache; updates indices.
 *
 * @note This repository can be used only after ArtistRepository is fully loaded.
 */
class AlbumRepository: public Repository<AlbumData, domain::Album> {

public:
    /**
     * @brief Constructor.
     *
     * @param ampache Used for communication with Ampache server.
     * @param cache Used for accessing the disk cache.
     * @param artistRepository Used to set artist of the album.
     * @param indices Indices to update.
     */
    explicit AlbumRepository(Ampache& ampache, Cache& cache, Indices& indices,
        const ArtistRepository* const artistRepository);

    ~AlbumRepository();

    AlbumRepository(const AlbumRepository& other) = delete;

    AlbumRepository& operator=(const AlbumRepository& other) = delete;

    /**
     * @brief Event fired when some album arts were loaded.
     *
     * @sa loadArts()
     */
    infrastructure::Event<std::pair<int, int>> artsLoaded{};

    /**
     * @brief Event fired when all albums arts were loaded.
     *
     * @param bool true if an error occured, false otherwise
     */
    infrastructure::Event<bool> artsFullyLoaded{};

    /**
     * @brief Event fired when further loading of album arts was disabled.
     *
     * @sa disableLoading()
     */
    infrastructure::Event<void> artsLoadingDisabled{};

    /**
     * @brief Sets the which provider should be used to load data.
     *
     * @note maxCount() can change when setting the provider type.
     *
     * @param providerType The type of provider that shall be used to load data.
     *
     * @sa ::providerChanged
     */
    void setProviderType(ProviderType providerType) override;

    /**
     * @brief Get album data with the given ID.
     *
     * @param id The ID of the album data that shall be returned.
     * @return Album data with the given @p id
     *
     * @sa domain::AlbumData::getId()
     */
    AlbumData& getAlbumDataById(const std::string& id) const;

    /**
     * @brief Trigger load of album arts from Ampache server or the cache.
     *
     * @param filteredOffset Starting offset.  It takes filtering into account.  If no filter is set then it is the
     *        same as @p offset in the load() function.
     * @param count Number of album arts to load.
     * @return true if loading was triggered, false otherwise.
     *
     * @sa ::artsLoaded
     */
    bool loadArts(int filteredOffset, int count);

    void disableLoading() override;

protected:
    void onFilterChanged() override;

    void requestDataLoad(int offset, int limit) override;

    domain::Album& getDomainObject(const AlbumData& dataItem) const override;

    infrastructure::Event<std::vector<std::unique_ptr<AlbumData>>>& getDataLoadRequestFinishedEvent() override;

    void loadDataFromCache() override;

    void saveDataToCache() override;

    void handleLoadedItem(const AlbumData& dataItem) const override;

    void updateIndices(const std::vector<std::unique_ptr<AlbumData>>& data) override;

    void clear() override;

    void clearIndices() override;

    int getMaxDataSize() const override;

private:
    const ArtistRepository* const myArtistRepository = nullptr;

    // number of loaded arts so far
    int myArtsLoadProgress = 0;

    // starting offset and number of album arts that are being currently loaded; -1 if no arts loading
    // is in progress
    int myArtsLoadOffset = -1;
    int myArtsLoadCount = -1;

    void onAmpacheReadyArts(const std::map<std::string, QPixmap>& arts);
    void onCacheReadyArts(const std::map<std::string, QPixmap>& arts);

    domain::Album* findFilteredAlbumById(const std::string& id, int offset, int count) const;
    bool raiseEmptyIfResultNotValid();
};

}

#endif // ALBUMREPOSITORY_H
