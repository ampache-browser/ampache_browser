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
template <typename T>
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

    /**
     * @brief Sets the data that shall be filtered.
     *
     * @param sourceData The data that shall be filtered.
     */
    virtual void setSourceData(const std::vector<std::unique_ptr<T>>& sourceData);

    /**
     * @brief Notifies the instance that source data at the given offset and length has changed.
     *
     * @param offset The starting offset of the changed data. -1 if all source data has changed.
     * @param length The number of changed records.
     */
    virtual void processUpdatedSourceData(int offset = -1, int length = -1);

    /**
     * @brief Gets the result of the filter.
     *
     * @return Filtered data.
     */
    const std::vector<std::reference_wrapper<T>>& getFilteredData() const;

protected:
    // stores source data
    const std::vector<std::unique_ptr<T>>* mySourceData{};

    // stores the result of the filter
    std::vector<std::reference_wrapper<T>> myFilteredData;
};

}



#include "src/data/filters/filter.cc"



#endif // FILTER_H
