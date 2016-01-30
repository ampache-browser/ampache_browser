// artist_filter_for_albums.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ARTISTFILTERFORALBUMS_H
#define ARTISTFILTERFORALBUMS_H



#include <vector>
#include <memory>
#include "filter.h"



namespace domain {
class Artist;
}



namespace data {

class AlbumData;
class Indices;



class ArtistFilterForAlbums: public Filter<AlbumData> {

public:
    explicit ArtistFilterForAlbums(const std::vector<std::reference_wrapper<const domain::Artist>>& artists,
        Indices& indices);

    ~ArtistFilterForAlbums() override;

    void apply() override;

private:
    const std::vector<std::reference_wrapper<const domain::Artist>> myArtists;
    Indices& myIndices;

    void onIndexChanged();
};

}



#endif // ARTISTFILTERFORALBUMS_H
