// name_filter_for_albums.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef NAMEFILTERFORALBUMS_H
#define NAMEFILTERFORALBUMS_H



#include <vector>
#include <memory>
#include "filter.h"



namespace data {

class AlbumData;



class NameFilterForAlbums: public Filter<AlbumData> {

public:
    explicit NameFilterForAlbums(const std::string& namePattern);

    void apply() override;

private:
    const std::string myNamePattern;
};

}



#endif // NAMEFILTERFORALBUMS_H
