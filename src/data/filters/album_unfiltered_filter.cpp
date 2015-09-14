// album_unfiltered_filter.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include "data/filters/album_unfiltered_filter.h"



namespace data {

void AlbumUnfilteredFilter::apply() {
    auto offset = myFilteredData.size();
    auto length = mySourceData->size() - myFilteredData.size();
    for (auto idx = offset; idx < offset + length; idx++) {
        myFilteredData.push_back(*(*mySourceData)[idx]);
    }

    Filter<AlbumData>::apply();
}

}
