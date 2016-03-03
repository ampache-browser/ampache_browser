// unfiltered_filter.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



namespace data {

template <class T>
void UnfilteredFilter<T>::setSourceData(const std::vector<std::unique_ptr<T>>& sourceData) {
    Filter<T>::setSourceData(sourceData);
    initializeFilteredData();
}



template <class T>
void UnfilteredFilter<T>::apply() {
    // do not fire changed event
}



template <class T>
void UnfilteredFilter<T>::processUpdatedSourceData(int offset, int length) {
    if (offset != -1) {
        // resize
        for (int idx = this->myFilteredData.size(); idx < offset + length; idx++) {
            this->myFilteredData.push_back(*(*this->mySourceData)[idx]);
        }

        // update with new references
        for (auto idx = offset; idx < offset + length; idx++) {
            this->myFilteredData[idx] = *(*this->mySourceData)[idx];
        }
    } else {
        initializeFilteredData();
    }

    this->changed();
}



template <class T>
void UnfilteredFilter<T>::initializeFilteredData() {
    this->myFilteredData.clear();
    for (auto& source: *this->mySourceData) {
        this->myFilteredData.push_back(*source);
    }
}

}
