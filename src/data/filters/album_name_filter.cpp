// album_name_filter.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <vector>
#include <memory>
#include "../data_objects/album_data.h"
#include "data/filters/album_name_filter.h"

using namespace std;



namespace data {

AlbumNameFilter::AlbumNameFilter(const string& namePattern):
myNamePattern(namePattern) {
}



void AlbumNameFilter::apply() {
    myFilteredData.clear();

    for (auto& albumData: *mySourceData) {
        auto name = albumData->getAlbum().getName();
        if (search(name.begin(), name.end(), myNamePattern.begin(), myNamePattern.end(),
            [](char c1, char c2) {return toupper(c1) == toupper(c2); }) != name.end()) {

            myFilteredData.push_back(*albumData);
        }
    }

    Filter<AlbumData>::apply();
}

}
