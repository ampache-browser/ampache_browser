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
#include "infrastructure/event/event.h"



namespace data {

template <class T>
class Filter {

public:
    explicit Filter();

    virtual ~Filter();

    infrastructure::Event<void> changed{};

    void setSourceData(const std::vector<std::unique_ptr<T>>& sourceData);

    const std::vector<std::reference_wrapper<T>>& getFilteredData() const;

    virtual void apply();

protected:
    const std::vector<std::unique_ptr<T>>* mySourceData;
    std::vector<std::reference_wrapper<T>> myFilteredData;
};

}



#include "src/data/filters/filter.cpp"



#endif // FILTER_H
