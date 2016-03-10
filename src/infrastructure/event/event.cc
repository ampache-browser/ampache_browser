// event.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <functional>
#include <algorithm>
#include "infrastructure/event/delegate.h"



namespace infrastructure {

template <typename T>
void Event<T>::operator()(T& arg) {
    myIsDispatching = true;
    for (auto& subscriber: mySubscribers) {
        subscriber(arg);
    }
    myIsDispatching = false;

    for (auto& subscriber: myPendingRemovals) {
        removeSubscriber(subscriber);
    }
    myPendingRemovals.clear();
    for (auto& subscriber: myPendingAdditions) {
        mySubscribers.push_back(subscriber);
    }
    myPendingAdditions.clear();
}



template <typename T>
void Event<T>::operator+=(Delegate<T> subscriber) {
    if (myIsDispatching) {
        myPendingAdditions.push_back(subscriber);
    } else {
        mySubscribers.push_back(subscriber);
    }
}



template <typename T>
void Event<T>::operator-=(Delegate<T> subscriber) {
    if (myIsDispatching) {
        myPendingRemovals.push_back(subscriber);
    } else {
        removeSubscriber(subscriber);
    }
}



template <typename T>
void Event<T>::removeSubscriber(Delegate<T>& subscriber) {
    auto subscriberIter = std::find(mySubscribers.begin(), mySubscribers.end(), subscriber);
    if (subscriberIter != mySubscribers.end()) {
        mySubscribers.erase(subscriberIter);
    }
}

}
