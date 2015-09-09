// filter.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef FILTER_H
#define FILTER_H



#include <vector>
#include <memory>
#include "infrastructure/event.h"



namespace data {

template <class T>
class Filter {

public:
    Filter(std::vector<std::unique_ptr<T>>& sourceData);

    virtual ~Filter();

    infrastructure::Event<bool> changed{};

    std::vector<std::reference_wrapper<T>>& getFilteredData();

    virtual void apply();

protected:
    std::vector<std::unique_ptr<T>>& mySourceData;
    std::vector<std::reference_wrapper<T>> myFilteredData;
};

}



#include "filter.cpp"



#endif // FILTER_H
