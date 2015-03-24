// event.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef EVENT_H
#define EVENT_H



#include <vector>
#include <functional>

using namespace std;



namespace infrastructure {

/**
 * @brief Simple event dispatcher.
 *
 */
template <class T>
class Event {

public:

    /**
     * @brief Fires the event
     *
     * @param args Event arguments
     * @return void
     */
    void operator()(T& args);

    /**
     * @brief Subscribes a subscriber (listener) to the event.
     *
     * @param subscriber A listener that shall be subscribed to the event.
     * @return void
     */
    void operator+=(function<void(T&)> subscriber);

    /**
     * @brief Unsubscribes the subscriber from the event.
     *
     * @param subscriber A listener that shall be unsubscribed from the event.
     * @return void
     */
    void operator-=(function<void(T&)> subscriber);

private:
    vector<function<void(T&)>> mySubscribers;

};

}



#include "src/infrastructure/event.cpp"



#endif // EVENT_H
