// delegate_void.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <functional>
#include <string>

#include "infrastructure/event/delegate.h"



namespace infrastructure {

Delegate<void>::Delegate(const std::string& name, const void* const instance, std::function<void()> function):
myName(name),
myInstance(instance),
myFunction(function) {
}



void Delegate<void>::operator()() const {
    myFunction();
}



std::string Delegate<void>::name() const {
    return myName;
}



const void* Delegate<void>::instance() const {
    return myInstance;
}

}
