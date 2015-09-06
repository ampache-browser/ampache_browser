// indices.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef INDICES_H
#define INDICES_H



#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "infrastructure/event.h"
#include "domain/artist.h"
#include "../../src/data/data_objects/album_data.h"



namespace data {

class TrackData;



using ArtistAlbumIndex = std::unordered_map<
    std::reference_wrapper<const domain::Artist>,
    std::unordered_set<std::reference_wrapper<AlbumData>, std::hash<AlbumData>>,
    std::hash<domain::Artist>>;



class Indices {

public:
    infrastructure::Event<bool> changed{};

    ArtistAlbumIndex& getArtistAlbum();

    void updateArtistAlbum(domain::Artist& artist, AlbumData& albumData);

private:
    ArtistAlbumIndex myArtistAlbum;
};

}



#endif // INDICES_H
