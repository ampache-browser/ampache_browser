// album_name_filter.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ALBUMNAMEFILTER_H
#define ALBUMNAMEFILTER_H



#include <vector>
#include <memory>
#include "filter.h"



namespace data {

class AlbumData;



class AlbumNameFilter: public Filter<AlbumData> {

public:
    AlbumNameFilter(const std::string& namePattern);

    void apply() override;

private:
    std::string myNamePattern;
};

}



#endif // ALBUMNAMEFILTER_H
