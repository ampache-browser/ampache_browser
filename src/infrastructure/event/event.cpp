// event.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <functional>
#include <algorithm>
#include "infrastructure/event/delegate.h"



namespace infrastructure {

template <class T>
void Event<T>::operator()(T& arg) const {
    // SMELL: What happens if a subscriber unsubsribes itself wihile handling the event?
    for (auto& subscriber: mySubscribers) {
        subscriber(arg);
    }
}



template <class T>
void Event<T>::operator+=(Delegate<T> subscriber) {
    mySubscribers.push_back(subscriber);
}



template <class T>
void Event<T>::operator-=(Delegate<T> subscriber) {
    auto subscriberIter = std::find(mySubscribers.begin(), mySubscribers.end(), subscriber);
    if (subscriberIter != mySubscribers.end()) {
        mySubscribers.erase(subscriberIter);
    }
}

}
