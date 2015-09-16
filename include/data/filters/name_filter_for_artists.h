// name_filter_for_artists.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef NAMEFILTERFORARTISTS_H
#define NAMEFILTERFORARTISTS_H



#include "filter.h"



namespace data {

class ArtistData;



class NameFilterForArtists: public Filter<ArtistData> {

public:
    explicit NameFilterForArtists(const std::string& namePattern);

    void apply() override;

private:
    std::string myNamePattern;
};

}



#endif // NAMEFILTERFORARTISTS_H
