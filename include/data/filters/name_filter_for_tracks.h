// name_filter_for_tracks.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef NAMEFILTERFORTRACKS_H
#define NAMEFILTERFORTRACKS_H



#include "filter.h"



namespace data {

class TrackData;



class NameFilterForTracks: public Filter<TrackData> {

public:
    explicit NameFilterForTracks(const std::string& namePattern);

    void apply() override;

private:
    std::string myNamePattern;
};

}



#endif // NAMEFILTERFORTRACKS_H
