// event_void.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <algorithm>
#include <vector>

#include "infrastructure/event/delegate.h"
#include "infrastructure/event/event.h"



namespace infrastructure {

void Event<void>::operator()() {
    for (auto& subscriber: std::vector<Delegate<void>>{mySubscribers}) {
        subscriber();
    }
    // no instance variable should be accessed here in order to allow destruction of objects that fired an event during
    // handling it
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
