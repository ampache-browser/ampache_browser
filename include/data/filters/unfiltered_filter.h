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
template <class T>
class UnfilteredFilter: public Filter<T> {

public:
    /**
     * @sa Filter::apply()
     */
    void setSourceData(const std::vector<std::unique_ptr<T>>& sourceData) override;

    /**
     * @sa Filter::apply()
     */
    void apply() override;

    /**
     * @brief Notifies the instance that source data at the given offset and length has changed.
     *
     * @param offset The starting offset of the changed data. -1 if all source data has changed.
     * @param length The number of changed records.
     */
    void processUpdatedSourceData(int offset, int length);

private:
    void initializeFilteredData();
};

}



#include "src/data/filters/unfiltered_filter.cc"



#endif // ALBUMUNFILTEREDFILTER_H
