// unfiltered_filter.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



namespace data {

template <class T>
void UnfilteredFilter<T>::apply() {
}



template <class T>
void UnfilteredFilter<T>::processUpdatedSourceData(int offset, int length) {
    // resize
    for (auto idx = this->myFilteredData.size(); idx < offset + length; idx++) {
        this->myFilteredData.push_back(*(*this->mySourceData)[idx]);
    }

    // update with new references
    for (auto idx = offset; idx < offset + length; idx++) {
        this->myFilteredData[idx] = (*(*this->mySourceData)[idx]);
    }

    this->changed();
}

}
