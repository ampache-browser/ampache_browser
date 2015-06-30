// artist_repository.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ARTISTREPOSITORY_H
#define ARTISTREPOSITORY_H



#include <string>
#include <vector>
#include <memory>

#include "infrastructure/event.h"
#include "../../src/data/artist_data.h"
#include "domain/artist.h"



namespace data {

class AmpacheService;



class ArtistRepository {

public:
    explicit ArtistRepository(AmpacheService& ampacheService);

    infrastructure::Event<std::pair<int, int>> loaded{};

    bool load(int offset, int limit);

    domain::Artist& get(int offset) const;

    domain::Artist& getById(const std::string& id) const;

    bool isLoaded(int offset, int limit = 1) const;
    
    int maxCount() const;

private:
    std::vector<std::unique_ptr<ArtistData>> myArtistsData;
    AmpacheService& myAmpacheService;
    int myLoadOffset = -1;

    void onReadyArtists(std::vector<std::unique_ptr<ArtistData>>& artistData);
};

}

#endif // ARTISTREPOSITORY_H
