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

    ArtistRepository(const ArtistRepository& other) = delete;

    ArtistRepository& operator=(const ArtistRepository& other) = delete;

    infrastructure::Event<std::pair<int, int>> loaded{};

    infrastructure::Event<bool> fullyLoaded{};

    infrastructure::Event<bool> filterChanged{};

    bool load(int offset, int limit);

    domain::Artist& get(int filteredOffset) const;

    domain::Artist& getById(const std::string& id) const;

    ArtistData& getArtistDataById(const std::string& id) const;

    bool isLoaded(int filteredOffset, int limit = 1) const;
    
    int maxCount();

    void setNameFilter(const std::string& namePattern);

    void unsetFilter();

private:
    std::vector<std::unique_ptr<ArtistData>> myArtistsData;
    std::vector<std::reference_wrapper<ArtistData>> myArtistDataReferences;
    std::vector<std::reference_wrapper<ArtistData>> myStoredArtistDataReferences;
    AmpacheService& myAmpacheService;
    int myLoadProgress = 0;
    int myLoadOffset = -1;
    bool myIsFilterSet = false;
    int myCachedMaxCount = -1;

    void onReadyArtists(std::vector<std::unique_ptr<ArtistData>>& artistData);

    int computeMaxCount() const;
};

}

#endif // ARTISTREPOSITORY_H
