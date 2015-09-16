// artist_name_filter.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include "../data_objects/artist_data.h"
#include "data/filters/name_filter_for_artists.h"

using namespace std;



namespace data {

NameFilterForArtists::NameFilterForArtists(const string& namePattern):
myNamePattern(namePattern) {
}



void NameFilterForArtists::apply() {
    myFilteredData.clear();

    for (auto& artistData: *mySourceData) {
        auto name = artistData->getArtist().getName();
        if (search(name.begin(), name.end(), myNamePattern.begin(), myNamePattern.end(),
            [](char c1, char c2) {return toupper(c1) == toupper(c2); }) != name.end()) {

            myFilteredData.push_back(*artistData);
        }
    }

    Filter<ArtistData>::apply();
}

}
