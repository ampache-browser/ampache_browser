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



namespace data {

class AmpacheService;
class ArtistRepository;



class AlbumRepository {

public:
    explicit AlbumRepository(AmpacheService& ampacheService);

    infrastructure::Event<std::pair<int, int>> loaded{};

    infrastructure::Event<std::pair<int, int>> artsLoaded{};

    bool load(int offset, int limit);

//     std::vector<std::reference_wrapper<domain::Album>> get(int offset, int limit);

    domain::Album& get(int offset) const;

    // TODO: This will be internal. (ID maybe will not be part of domain model)
//     domain::Album& getById(const std::string& id);

    std::vector<std::reference_wrapper<domain::Album>> getByArtist(const domain::Artist& artist) const;

    bool loadArts(int offset, int limit);

    void populateArtists(const ArtistRepository& artistRepository);

    bool isLoaded(int offset, int limit = 1) const;

//     int count() const;

    int maxCount() const;

private:
    std::vector<std::unique_ptr<AlbumData>> myAlbumsData;
    AmpacheService& myAmpacheService;
    int myLoadOffset = -1;
    int myArtsLoadOffset = -1;

    void onReadyAlbums(std::vector<std::unique_ptr<AlbumData>>& albumsData);
    void onReadyArts(std::map<std::string, QPixmap>& arts);
};

}

#endif // ALBUMREPOSITORY_H
