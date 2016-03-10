// filter.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <vector>
#include <memory>



namespace data {

template <typename T>
Filter<T>::Filter() {
}



template <typename T>
Filter<T>::~Filter() {
}



template <typename T>
void Filter<T>::setSourceData(const std::vector<std::unique_ptr<T>>& sourceData) {
    mySourceData = &sourceData;
}



template <typename T>
const std::vector<std::reference_wrapper<T>>& Filter<T>::getFilteredData() const {
    return myFilteredData;
}



template <typename T>
void Filter<T>::apply() {
    changed();
}

}
