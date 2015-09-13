// album_artist_filter.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ALBUMARTISTFILTER_H
#define ALBUMARTISTFILTER_H



#include <vector>
#include <memory>
#include "filter.h"



namespace domain {
class Artist;
}



namespace data {

class AlbumData;
class Indices;



class AlbumArtistFilter: public Filter<AlbumData> {

public:
    AlbumArtistFilter(std::vector<std::reference_wrapper<const domain::Artist>> artists, Indices& indices);

    ~AlbumArtistFilter() override;

    void apply() override;

private:
    std::vector<std::reference_wrapper<const domain::Artist>> myArtists;
    Indices& myIndices;

    void onIndexChanged();
};

}



#endif // ALBUMARTISTFILTER_H
