// event_void.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <functional>
#include <algorithm>
#include "infrastructure/event/event.h"



namespace infrastructure {

void Event<void>::operator()() {
    myIsDispatching = true;
    for (auto& subscriber: mySubscribers) {
        subscriber();
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



void Event<void>::operator+=(Delegate<void> subscriber) {
    if (myIsDispatching) {
        myPendingAdditions.push_back(subscriber);
    } else {
        mySubscribers.push_back(subscriber);
    }
}



void Event<void>::operator-=(Delegate<void> subscriber) {
    if (myIsDispatching) {
        myPendingRemovals.push_back(subscriber);
    } else {
        removeSubscriber(subscriber);
    }
}



void Event<void>::removeSubscriber(Delegate<void>& subscriber) {
    auto subscriberIter = std::find(mySubscribers.begin(), mySubscribers.end(), subscriber);
    if (subscriberIter != mySubscribers.end()) {
        mySubscribers.erase(subscriberIter);
    }
}

}
