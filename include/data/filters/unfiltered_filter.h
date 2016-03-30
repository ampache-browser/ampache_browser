// unfiltered_filter.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef UNFILTEREDFILTER_H
#define UNFILTEREDFILTER_H



#include <vector>
#include <memory>
#include "filter.h"



namespace data {

/**
 * @brief Filter that does not do any filtering.
 */
template <typename T>
class UnfilteredFilter: public Filter<T> {

public:
    void processUpdatedSourceData(int offset = -1, int length = -1) override;

private:
    void initializeFilteredData();
};

}



#include "src/data/filters/unfiltered_filter.cc"



#endif // ALBUMUNFILTEREDFILTER_H
