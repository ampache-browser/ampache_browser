// name_filter_for_albums.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include "../data_objects/album_data.h"
#include "data/filters/name_filter_for_albums.h"

using namespace std;



namespace data {

NameFilterForAlbums::NameFilterForAlbums(const string& namePattern):
myNamePattern(namePattern) {
}



void NameFilterForAlbums::processUpdatedSourceData(int offset, int length) {
    myFilteredData.clear();

    for (auto& albumData: *mySourceData) {
        if (albumData == nullptr) {
            continue;
        }
        auto name = albumData->getAlbum().getName();
        if (search(name.begin(), name.end(), myNamePattern.begin(), myNamePattern.end(),
            [](char c1, char c2) {return toupper(c1) == toupper(c2); }) != name.end()) {

            myFilteredData.push_back(*albumData);
        }
    }

    Filter<AlbumData>::processUpdatedSourceData(offset, length);
}

}
