// album_unfiltered_filter.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ALBUMUNFILTEREDFILTER_H
#define ALBUMUNFILTEREDFILTER_H



#include <vector>
#include <memory>
#include "filter.h"



namespace data {

class AlbumData;



class AlbumUnfilteredFilter: public Filter<AlbumData> {

public:
    void apply() override;
};

}



#endif // ALBUMUNFILTEREDFILTER_H
