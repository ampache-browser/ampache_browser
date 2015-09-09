// filter.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <vector>
#include <memory>



namespace data {

template <class T>
Filter<T>::Filter(std::vector<std::unique_ptr<T>>& sourceData):
mySourceData(sourceData) {
}



template<class T>
Filter<T>::~Filter() {
}



template <class T>
std::vector<std::reference_wrapper<T>>& Filter<T>::getFilteredData() {
    return myFilteredData;
}



template <class T>
void Filter<T>::apply() {
    bool b = false;
    changed(b);
}

}
