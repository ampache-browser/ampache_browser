// index_types.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef INDEXTYPES_H
#define INDEXTYPES_H



#include <vector>
#include <unordered_map>
#include <memory>



namespace data {

class AlbumData;
class TrackData;



using ArtistAlbumVectorIndex = std::unordered_map<
    std::reference_wrapper<const domain::Artist>,
    std::vector<std::reference_wrapper<AlbumData>>,
    std::hash<domain::Artist>>;

}



#endif // INDEXTYPES_H
