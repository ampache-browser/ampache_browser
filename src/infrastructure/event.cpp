// event.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <iostream>

#include <functional>
#include <algorithm>
#include "event.h"

using namespace std;



namespace infrastructure {

template <class T>
void Event<T>::operator()(T& args) {
    // SMELL: What happens if a subscriber unsubsribes itself wihile handling the event?
    for (auto& subscriber: mySubscribers) {
        subscriber(args);
    }
}



template <class T>
void Event<T>::operator+=(std::function<void(T&)> subscriber) {
    mySubscribers.push_back(subscriber);
}



// TODO: Implement.
template <class T>
void Event<T>::operator-=(std::function<void(T&)>) {
//     int idx = 0;
//     for (auto& sub: mySubscribers) {
//         if (sub == subscriber) {
//             mySubscribers.erase(mySubscribers.begin() + idx);
//         }
//         idx++;
//     }

//     auto subscriberIter = find(mySubscribers.begin(), mySubscribers.end(), subscriber);
//     if (subscriberIter != mySubscribers.end()) {
//         mySubscribers.erase(subscriberIter);
//     }
}

}
