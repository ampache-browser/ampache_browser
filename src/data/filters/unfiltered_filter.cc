// unfiltered_filter.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



namespace data {

template <typename T>
void UnfilteredFilter<T>::processUpdatedSourceData(int offset, int length) {
    if (length == 0) {
        return;
    }

    if (offset != -1) {
        // resize
        for (int idx = this->myFilteredData.size(); idx < offset + length; idx++) {
            this->myFilteredData.push_back((*this->mySourceData)[idx].get());
        }

        // update with new references
        for (auto idx = offset; idx < offset + length; idx++) {
            this->myFilteredData[idx] = (*this->mySourceData)[idx].get();
        }
    } else {
        initializeFilteredData();
    }

    Filter<T>::processUpdatedSourceData(offset, length);
}



template <typename T>
void UnfilteredFilter<T>::initializeFilteredData() {
    this->myFilteredData.clear();
    for (auto& source: *this->mySourceData) {
        this->myFilteredData.push_back(source.get());
    }
}

}
