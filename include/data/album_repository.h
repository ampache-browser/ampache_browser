// album_repository.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ALBUMREPOSITORY_H
#define ALBUMREPOSITORY_H



#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "infrastructure/event.h"
#include "../../src/data/album_data.h"
#include "domain/album.h"
#include "domain/artist.h"
#include "../../src/data/index_types.h"



namespace data {

class AmpacheService;
class ArtistRepository;



class AlbumRepository {

public:
    explicit AlbumRepository(AmpacheService& ampacheService, ArtistRepository& artistRepository);

    AlbumRepository(const AlbumRepository& other) = delete;

    AlbumRepository& operator=(const AlbumRepository& other) = delete;

    infrastructure::Event<std::pair<int, int>> loaded{};

    infrastructure::Event<std::pair<int, int>> artsLoaded{};

    infrastructure::Event<bool> fullyLoaded{};

    infrastructure::Event<bool> filterChanged{};

    bool load(int offset, int limit);

//     std::vector<std::reference_wrapper<domain::Album>> get(int offset, int limit);

    domain::Album& get(int filteredOffset) const;

    domain::Album& getById(const std::string& id) const;

//     std::vector<std::reference_wrapper<domain::Album>> getByArtist(const domain::Artist& artist) const;

    AlbumData& getAlbumDataById(const std::string& id) const;

    bool loadArts(int offset, int limit);

    void populateArtists(const ArtistRepository& artistRepository);

    bool isLoaded(int filteredOffset, int limit = 1) const;

//     int count() const;

    int maxCount() const;

    void setArtistFilter(const domain::Artist& artist);

    void unsetArtistFilter();

    void setArtistIndex(std::unique_ptr<ArtistAlbumVectorIndex> artistIndex);

private:
    std::vector<std::unique_ptr<AlbumData>> myAlbumsData;
    std::vector<std::reference_wrapper<AlbumData>> myAlbumDataReferences;
    std::unique_ptr<ArtistAlbumVectorIndex> myArtistIndex = nullptr;
    AmpacheService& myAmpacheService;
    ArtistRepository& myArtistRepository;
    int myLoadProgress = 0;
    int myLoadOffset = -1;
    int myArtsLoadOffset = -1;
    const domain::Artist* myCurrentArtistFilter = nullptr;

    void onReadyAlbums(std::vector<std::unique_ptr<AlbumData>>& albumsData);
    void onReadyArts(std::map<std::string, QPixmap>& arts);
};

}

#endif // ALBUMREPOSITORY_H
