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

#include "infrastructure/event/event.h"
#include "../../src/data/data_objects/artist_data.h"
#include "filters/filter.h"
#include "filters/unfiltered_filter.h"
#include "domain/artist.h"



namespace data {

class AmpacheService;
class Cache;



class ArtistRepository {

public:
    explicit ArtistRepository(AmpacheService& ampacheService, Cache& cache);

    ~ArtistRepository();

    ArtistRepository(const ArtistRepository& other) = delete;

    ArtistRepository& operator=(const ArtistRepository& other) = delete;

    infrastructure::Event<std::pair<int, int>> loaded{};

    infrastructure::Event<void> fullyLoaded{};

    infrastructure::Event<void> filterChanged{};

    bool load(int offset, int limit);

    domain::Artist& get(int filteredOffset) const;

    domain::Artist& getById(const std::string& id) const;

    bool isLoaded(int filteredOffset, int limit = 1) const;
    
    int maxCount();

    void setFilter(std::unique_ptr<Filter<ArtistData>> filter);

    void unsetFilter();

    bool isFiltered() const;

private:
    std::vector<std::unique_ptr<ArtistData>> myArtistsData;
    AmpacheService& myAmpacheService;
    Cache& myCache;
    int myLoadProgress = 0;
    int myLoadOffset = -1;
    std::shared_ptr<UnfilteredFilter<ArtistData>> myUnfilteredFilter = std::shared_ptr<UnfilteredFilter<ArtistData>>{
        new UnfilteredFilter<ArtistData>{}};
    std::shared_ptr<Filter<ArtistData>> myFilter = nullptr;
    bool myIsFilterSet = false;
    int myCachedMaxCount = -1;

    void onReadyArtists(std::vector<std::unique_ptr<ArtistData>>& artistData);
    void onFilterChanged();

    void loadFromCache();
    int computeMaxCount() const;
};

}

#endif // ARTISTREPOSITORY_H
