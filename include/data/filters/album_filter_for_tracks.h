// album_filter_for_tracks.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ALBUMFILTERFORTRACKS_H
#define ALBUMFILTERFORTRACKS_H



#include <vector>
#include <memory>
#include "filter.h"



namespace domain {
class Album;
}



namespace data {

class TrackData;
class Indices;



class AlbumFilterForTracks: public Filter<TrackData> {

public:
    explicit AlbumFilterForTracks(const std::vector<std::reference_wrapper<const domain::Album>>& albums,
        Indices& indices);

    ~AlbumFilterForTracks() override;

    void apply() override;

private:
    const std::vector<std::reference_wrapper<const domain::Album>> myAlbums;
    Indices& myIndices;

    void onIndexChanged();
};

}



#endif // ALBUMFILTERFORTRACKS_H
