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

void Event<void>::operator()() const {
    // SMELL: What happens if a subscriber unsubsribes itself wihile handling the event?
    for (auto& subscriber: mySubscribers) {
        subscriber();
    }
}



void Event<void>::operator+=(Delegate<void> subscriber) {
    mySubscribers.push_back(subscriber);
}



void Event<void>::operator-=(Delegate<void> subscriber) {
    auto subscriberIter = std::find(mySubscribers.begin(), mySubscribers.end(), subscriber);
    if (subscriberIter != mySubscribers.end()) {
        mySubscribers.erase(subscriberIter);
    }
}

}
