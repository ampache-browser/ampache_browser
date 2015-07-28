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
#include "../../src/data/track_data.h"
#include "../../src/data/album_data.h"
#include "domain/track.h"
#include "domain/album.h"
#include "domain/artist.h"
#include "../../src/data/index_types.h"



namespace data {

class AmpacheService;
class ArtistRepository;
class AlbumRepository;



class TrackRepository {

public:
    explicit TrackRepository(data::AmpacheService& ampacheService, data::ArtistRepository& artistRepository,
        data::AlbumRepository& albumRepository);

    TrackRepository(const TrackRepository& other) = delete;

    TrackRepository& operator=(const TrackRepository& other) = delete;

    infrastructure::Event<std::pair<int, int>> loaded{};

    infrastructure::Event<bool> fullyLoaded{};

    infrastructure::Event<bool> filterChanged{};

    bool load(int offset, int limit);

    domain::Track& get(int filteredOffset) const;

    std::vector<std::reference_wrapper<domain::Track>> getByAlbum(const domain::Album& album) const;

    std::vector<std::reference_wrapper<domain::Track>> getByArtist(const domain::Artist& artist) const;

    std::unique_ptr<ArtistAlbumVectorIndex> getArtistAlbumIndex();

    void populateArtists(const ArtistRepository& artistRepository);

    void populateAlbums(const AlbumRepository& albumRepository);

    bool isLoaded(int filteredOffset, int limit = 1) const;

    int maxCount();

    void setArtistFilter(std::vector<std::reference_wrapper<const domain::Artist>> artists);

    void unsetArtistFilter();

    void setAlbumFilter(std::vector<std::reference_wrapper<const domain::Album>> albums);

    void unsetAlbumFilter();

private:
    std::vector<std::unique_ptr<TrackData>> myTracksData;
    std::vector<std::reference_wrapper<TrackData>> myTrackDataReferences;
    std::vector<std::reference_wrapper<TrackData>> myStoredTrackDataReferences;
    std::unordered_map<
        std::reference_wrapper<const domain::Artist>,
        std::unordered_set<std::reference_wrapper<AlbumData>, std::hash<data::AlbumData>>,
        std::hash<domain::Artist>> myArtistAlbumIndex;
    std::unordered_map<
        std::reference_wrapper<const domain::Artist>,
        std::vector<std::reference_wrapper<TrackData>>,
        std::hash<domain::Artist>> myArtistTrackIndex;
    std::unordered_map<
        std::reference_wrapper<const domain::Album>,
        std::vector<std::reference_wrapper<TrackData>>,
        std::hash<domain::Album>> myAlbumTrackIndex;
    AmpacheService& myAmpacheService;
    ArtistRepository& myArtistRepository;
    AlbumRepository& myAlbumRepository;
    int myLoadProgress = 0;
    int myLoadOffset = -1;
    std::vector<std::reference_wrapper<const domain::Artist>> myCurrentArtistFilter;
    std::vector<std::reference_wrapper<const domain::Album>> myCurrentAlbumFilter;
    int myCachedMaxCount = -1;

    void onReadyTracks(std::vector<std::unique_ptr<TrackData>>& trackData);

    void updateIndicies(TrackData& trackData);

    int computeMaxCount() const;
};

}

#endif // TRACKREPOSITORY_H
