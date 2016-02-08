// name_filter_for_artists.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef NAMEFILTERFORARTISTS_H
#define NAMEFILTERFORARTISTS_H



#include "filter.h"



namespace data {

class ArtistData;



/**
 * @brief Provides artists data filtered by name.
 *
 * Filter is making exact, case insensitive match of the artist's name (domain::Album::getName()) with the given
 * pattern.
 */
class NameFilterForArtists: public Filter<ArtistData> {

public:
    /**
     * @brief Constructor.
     *
     * @param namePattern The pattern that shall be used to match the artist's name.
     */
    explicit NameFilterForArtists(const std::string& namePattern);

    /**
     * @sa Filter::apply()
     */
    void apply() override;

private:
    const std::string myNamePattern;
};

}



#endif // NAMEFILTERFORARTISTS_H
