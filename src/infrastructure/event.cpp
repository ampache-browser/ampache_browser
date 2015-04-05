// event.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <functional>
#include "event.h"



namespace infrastructure {

template <class T>
void Event<T>::operator()(T& args) {
    for (auto subscriber: mySubscribers) {
        subscriber(args);
    }
}



template <class T>
void Event<T>::operator+=(std::function<void(T&)> subscriber) {
    mySubscribers.push_back(subscriber);
}



template <class T>
void Event<T>::operator-=(std::function<void(T&)> subscriber) {
    mySubscribers.erase(subscriber);
}

}
