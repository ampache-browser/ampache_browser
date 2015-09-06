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

#include "infrastructure/event.h"
#include "../../src/data/data_objects/track_data.h"
#include "../../src/data/data_objects/album_data.h"
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

    TrackRepository(const TrackRepository& other) = delete;

    TrackRepository& operator=(const TrackRepository& other) = delete;

    infrastructure::Event<std::pair<int, int>> loaded{};

    infrastructure::Event<bool> fullyLoaded{};

    infrastructure::Event<bool> filterChanged{};

    bool load(int offset, int limit);

    domain::Track& get(int filteredOffset) const;

    bool isLoaded(int filteredOffset, int limit = 1) const;

    int maxCount();

    void setArtistFilter(std::vector<std::reference_wrapper<const domain::Artist>> artists);

    void setAlbumFilter(std::vector<std::reference_wrapper<const domain::Album>> albums);

    void setNameFilter(const std::string& namePattern);

    void unsetFilter();

private:
    std::vector<std::unique_ptr<TrackData>> myTracksData;
    std::vector<std::reference_wrapper<TrackData>> myTrackDataReferences;
    std::vector<std::reference_wrapper<TrackData>> myStoredTrackDataReferences;
    std::unordered_map<
        std::reference_wrapper<const domain::Artist>,
        std::vector<std::reference_wrapper<TrackData>>,
        std::hash<domain::Artist>> myArtistTrackIndex;
    std::unordered_map<
        std::reference_wrapper<const domain::Album>,
        std::vector<std::reference_wrapper<TrackData>>,
        std::hash<domain::Album>> myAlbumTrackIndex;
    AmpacheService& myAmpacheService;
    Cache& myCache;
    ArtistRepository& myArtistRepository;
    AlbumRepository& myAlbumRepository;
    Indices& myIndices;
    int myLoadProgress = 0;
    int myLoadOffset = -1;
    bool myIsFilterSet = false;
    int myCachedMaxCount = -1;

    void onReadyTracks(std::vector<std::unique_ptr<TrackData>>& trackData);

    void updateIndicies(TrackData& trackData);
    void loadFromCache();
    int computeMaxCount() const;
};

}

#endif // TRACKREPOSITORY_H
