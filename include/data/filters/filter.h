// filter.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef FILTER_H
#define FILTER_H



#include <vector>
#include <memory>
#include "infrastructure/event/event.h"



namespace data {

/**
 * @brief Base class for filters.
 */
template <class T>
class Filter {

public:
    explicit Filter();

    virtual ~Filter();

    /**
     * @brief Fired when the result of the filted has changed.
     *
     * After this event getFilteredData() should be called to obtain the current result.
     */
    infrastructure::Event<void> changed{};

    // SMELL: Remove from this class because it is not commonality.
    /**
     * @brief Sets the data that shall be filtered.
     *
     * @param sourceData The data that shall be filtered.
     */
    virtual void setSourceData(const std::vector<std::unique_ptr<T>>& sourceData);

    /**
     * @brief Gets the result of the filter.
     *
     * @return Filtered data.
     */
    const std::vector<std::reference_wrapper<T>>& getFilteredData() const;

    /**
     * @brief Execute the filter.
     */
    virtual void apply();

protected:
    // stores source data
    const std::vector<std::unique_ptr<T>>* mySourceData{};

    // stores the result of the filter
    std::vector<std::reference_wrapper<T>> myFilteredData;
};

}



#include "src/data/filters/filter.cc"



#endif // FILTER_H
