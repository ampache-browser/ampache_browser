// unfiltered_filter.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef UNFILTEREDFILTER_H
#define UNFILTEREDFILTER_H



#include <vector>
#include <memory>
#include "filter.h"



namespace data {

template <class T>
class UnfilteredFilter: public Filter<T> {

public:
    void apply() override;

    void processUpdatedSourceData(int offset, int length);
};

}



#include "src/data/filters/unfiltered_filter.cpp"



#endif // ALBUMUNFILTEREDFILTER_H
