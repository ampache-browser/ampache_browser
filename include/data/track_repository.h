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
#include <memory>

#include "infrastructure/event.h"
#include "../../src/data/track_data.h"
#include "domain/track.h"
#include "domain/album.h"
#include "domain/artist.h"



namespace data {

class AmpacheService;
class ArtistRepository;
class AlbumRepository;



class TrackRepository {

public:
    explicit TrackRepository(AmpacheService& ampacheService);

    infrastructure::Event<std::pair<int, int>> loaded{};

    bool load(int offset, int limit);

    domain::Track& get(int offset) const;

    std::vector<std::reference_wrapper<domain::Track>> getByAlbum(const domain::Album& album) const;

    std::vector<std::reference_wrapper<domain::Track>> getByArtist(const domain::Artist& artist) const;

    void populateArtists(const ArtistRepository& artistRepository);

    void populateAlbums(const AlbumRepository& albumRepository);

    bool isLoaded(int offset, int limit = 1) const;

    int maxCount() const;

private:
    std::vector<std::unique_ptr<TrackData>> myTracksData;
    AmpacheService& myAmpacheService;
    int myLoadOffset = -1;

    void onReadyTracks(std::vector<std::unique_ptr<TrackData>>& trackData);
};

}

#endif // TRACKREPOSITORY_H
