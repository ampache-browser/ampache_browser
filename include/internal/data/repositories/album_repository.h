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
     * @param offsetAndCount Pair of offset and count which were requested to load by loadArts().  count is 0 if the
     * offset is no longer valid (e. g. in case the filter was changed in the meantime).
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

    void setProviderType(ProviderType providerType) override;

    /**
     * @brief Get a Album from AlbumData at the given offset.
     *
     * @param offset Offset of AlbumData that shall be queried for Album.
     * @return Album from AlbumData at the given @p offset.
     * @sa Repository<AlbumData, domain::Album>::get()
     */
    domain::Album& getUnfiltered(int offset) const;

    /**
     * @brief Get album data with the given ID.
     *
     * @param id The ID of the album data that shall be returned.
     * @return Album data with the given @p id
     *
     * @sa domain::AlbumData::getId()
     */
    AlbumData* getAlbumDataById(const std::string& id) const;

    /**
     * @brief Gets the load status of the given data items.
     *
     * @param offset Starting offset of the album data which load status shall be checked.
     * @param count Number of album data to check.
     * @return true if each specified album data is already loaded.
     */
    bool isLoadedUnfiltered(int offset, int count = 1) const;

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

    /**
     * @brief Trigger load of album arts from Ampache server or the cache using unfiltered offset.
     *
     * @param offset Starting offset.
     * @param count Number of album arts to load.
     * @return true if loading was triggered, false otherwise.
     *
     * @sa ::artsLoaded
     */
    bool loadArtsUnfiltered(int offset, int count);

    int maxCount() const override;

    void disableLoading() override;

protected:
    void requestDataLoad(int offset, int limit) override;

    domain::Album& getDomainObject(const AlbumData& dataItem) const override;

    infrastructure::Event<std::pair<std::vector<std::unique_ptr<AlbumData>>, bool>>& getDataLoadRequestFinishedEvent() override;

    void loadDataFromCache() override;

    void saveDataToCache() override;

    void handleLoadedItem(const AlbumData& dataItem) const override;

    void updateIndices(const std::vector<std::unique_ptr<AlbumData>>& data) override;

    void clear() override;

    void clearIndices() override;

    void handleFilterSetUnsetOrChanged() override;

private:
    const ArtistRepository* const myArtistRepository = nullptr;

    // number of loaded arts so far
    int myArtsLoadProgress = 0;

    // starting offset and number of album arts that are being currently loaded; -1 if no arts loading
    // is in progress
    int myArtsLoadOffset = -1;
    int myArtsLoadOffsetUnfiltered = -1;
    int myArtsLoadCount = -1;

    void onAmpacheReadyArts(const std::map<std::string, QPixmap>& arts);
    void onCacheReadyArts(const std::map<std::string, QPixmap>& arts);

    std::pair<std::map<std::string, QPixmap>, std::map<std::string, std::string>> setArts(
        const std::map<std::string, QPixmap>& arts);
    AlbumData* findAlbumDataById(const std::string& id, int filteredOffset, int count) const;
    AlbumData* findAlbumDataByIdUnfiltered(const std::string& id, int offset, int count) const;
    void fireArtsLoadedEvents();
};

}

#endif // ALBUMREPOSITORY_H
