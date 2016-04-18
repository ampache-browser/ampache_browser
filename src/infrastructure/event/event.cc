// event.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <functional>
#include <algorithm>
#include "infrastructure/event/delegate.h"



namespace infrastructure {

template <typename T>
void Event<T>::operator()(T& arg) {
    for (auto& subscriber: std::vector<Delegate<T>>{mySubscribers}) {
        subscriber(arg);
    }
    // no instance variable should be accessed here in order to allow destruction of objects that fired an event during
    // handling it
}



template <typename T>
void Event<T>::operator+=(Delegate<T> subscriber) {
    mySubscribers.push_back(subscriber);
}



template <typename T>
void Event<T>::operator-=(Delegate<T> subscriber) {
    auto subscriberIter = std::find(mySubscribers.begin(), mySubscribers.end(), subscriber);
    if (subscriberIter != mySubscribers.end()) {
        mySubscribers.erase(subscriberIter);
    }
}

}
