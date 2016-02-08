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



/**
 * @brief Provides tracks data filtered by albums.
 */
class AlbumFilterForTracks: public Filter<TrackData> {

public:
    /**
     * @brief Constructor.
     *
     * @param albums Albums by which the tracks shall be filtered.
     * @param indices Access to the data that shall be filtered.
     */
    explicit AlbumFilterForTracks(const std::vector<std::reference_wrapper<const domain::Album>>& albums,
        Indices& indices);

    ~AlbumFilterForTracks() override;

    /**
     * @sa Filter::apply()
     */
    void apply() override;

private:
    // arguments from the constructor
    const std::vector<std::reference_wrapper<const domain::Album>> myAlbums;
    Indices& myIndices;

    void onIndexChanged();
};

}



#endif // ALBUMFILTERFORTRACKS_H
