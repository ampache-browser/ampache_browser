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



class ArtistFilterForTracks: public Filter<TrackData> {

public:
    explicit ArtistFilterForTracks(const std::vector<std::reference_wrapper<const domain::Artist>>& artists,
        Indices& indices);

    ~ArtistFilterForTracks() override;

    void apply() override;

private:
    const std::vector<std::reference_wrapper<const domain::Artist>> myArtists;
    Indices& myIndices;

    void onIndexChanged();
};

}



#endif // ARTISTFILTERFORTRACKS_H
