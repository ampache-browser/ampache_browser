// event.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef EVENT_H
#define EVENT_H



#include <vector>
#include <functional>
#include "infrastructure/event/delegate.h"



namespace infrastructure {

/**
 * @brief Simple event dispatcher.
 */
template <typename T>
class Event {

public:

    /**
     * @brief Fires the event.
     *
     * @param arg Event argument.
     */
    void operator()(T& arg);

    /**
     * @brief Subscribes a subscriber (listener) to the event.
     *
     * @param subscriber A listener that shall be subscribed to the event.
     */
    void operator+=(Delegate<T> subscriber);

    /**
     * @brief Unsubscribes the subscriber from the event.
     *
     * @param subscriber A listener that shall be unsubscribed from the event.
     */
    void operator-=(Delegate<T> subscriber);

private:
    std::vector<Delegate<T>> mySubscribers;
};



/**
 * @brief Simple patrameter-less event dispatcher.
 */
template <>
class Event<void> {

public:

    /**
     * @brief Fires the event.
     */
    void operator()();

    /**
     * @brief Subscribes a subscriber (listener) to the event.
     *
     * @param subscriber A listener that shall be subscribed to the event.
     */
    void operator+=(Delegate<void> subscriber);

    /**
     * @brief Unsubscribes the subscriber from the event.
     *
     * @param subscriber A listener that shall be unsubscribed from the event.
     */
    void operator-=(Delegate<void> subscriber);

private:
    std::vector<Delegate<void>> mySubscribers;
};

}



#include "src/infrastructure/event/event.cc"



#endif // EVENT_H
