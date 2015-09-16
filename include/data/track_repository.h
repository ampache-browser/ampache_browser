// track_repository.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef TRACKREPOSITORY_H
#define TRACKREPOSITORY_H



#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "infrastructure/event/event.h"
#include "../../src/data/data_objects/track_data.h"
#include "../../src/data/data_objects/album_data.h"
#include "filters/filter.h"
#include "filters/unfiltered_filter.h"
#include "domain/track.h"
#include "domain/album.h"
#include "domain/artist.h"



namespace data {

class AmpacheService;
class Cache;
class ArtistRepository;
class AlbumRepository;
class Indices;



class TrackRepository {

public:
    explicit TrackRepository(AmpacheService& ampacheService, Cache& cache, ArtistRepository& artistRepository,
        AlbumRepository& albumRepository, Indices& indices);

    ~TrackRepository();

    TrackRepository(const TrackRepository& other) = delete;

    TrackRepository& operator=(const TrackRepository& other) = delete;

    infrastructure::Event<std::pair<int, int>> loaded{};

    infrastructure::Event<void> fullyLoaded{};

    infrastructure::Event<void> filterChanged{};

    bool load(int offset, int limit);

    domain::Track& get(int filteredOffset) const;

    bool isLoaded(int filteredOffset, int limit = 1) const;

    int maxCount();

    void setFilter(std::unique_ptr<Filter<TrackData>> filter);

    void unsetFilter();

    bool isFiltered() const;

private:
    std::vector<std::unique_ptr<TrackData>> myTracksData;
    AmpacheService& myAmpacheService;
    Cache& myCache;
    ArtistRepository& myArtistRepository;
    AlbumRepository& myAlbumRepository;
    Indices& myIndices;
    int myLoadProgress = 0;
    int myLoadOffset = -1;
    std::shared_ptr<UnfilteredFilter<TrackData>> myUnfilteredFilter = std::shared_ptr<UnfilteredFilter<TrackData>>{
        new UnfilteredFilter<TrackData>{}};
    std::shared_ptr<Filter<TrackData>> myFilter = nullptr;
    bool myIsFilterSet = false;
    int myCachedMaxCount = -1;

    void onReadyTracks(std::vector<std::unique_ptr<TrackData>>& trackData);
    void onFilterChanged();

    void updateIndicies(TrackData& trackData);
    void loadFromCache();
    int computeMaxCount() const;
};

}

#endif // TRACKREPOSITORY_H
