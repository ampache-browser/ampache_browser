// artist_filter_for_tracks.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ARTISTFILTERFORTRACKS_H
#define ARTISTFILTERFORTRACKS_H



#include <vector>
#include <memory>
#include "filter.h"



namespace domain {
class Artist;
}



namespace data {

class TrackData;
class Indices;



/**
 * @brief Provides tracks data filtered by artists.
 */
class ArtistFilterForTracks: public Filter<TrackData> {

public:
    /**
     * @brief Constructor.
     *
     * @param artists Artists by which the tracks shall be filtered.
     * @param indices Access to the data that shall be filtered.
     */
    explicit ArtistFilterForTracks(const std::vector<std::reference_wrapper<const domain::Artist>>& artists,
        Indices& indices);

    ~ArtistFilterForTracks() override;

    /**
     * @sa Filter::apply()
     */
    void apply() override;

private:
    // arguments from the constructor
    const std::vector<std::reference_wrapper<const domain::Artist>> myArtists;
    Indices& myIndices;

    void onIndexChanged();
};

}



#endif // ARTISTFILTERFORTRACKS_H
