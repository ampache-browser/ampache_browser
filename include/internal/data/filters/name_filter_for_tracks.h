// name_filter_for_tracks.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef NAMEFILTERFORTRACKS_H
#define NAMEFILTERFORTRACKS_H



#include "filter.h"



namespace data {

class TrackData;



/**
 * @brief Provides tracks data filtered by name.
 *
 * Filter is making exact, case insensitive match of the track's name (domain::Album::getName()) with the given pattern.
 *
 * Provided filtered data via ::getFilteredData() does not contain uninitialized (null) elements.
 */
class NameFilterForTracks: public Filter<TrackData> {

public:
    /**
     * @brief Constructor.
     *
     * @param namePattern The pattern that shall be used to match the track's name.
     */
    explicit NameFilterForTracks(const std::string& namePattern);

    void processUpdatedSourceData(int offset = -1, int length = -1) override;

private:
    const std::string myNamePattern;
};

}



#endif // NAMEFILTERFORTRACKS_H
