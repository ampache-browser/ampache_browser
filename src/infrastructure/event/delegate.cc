// delegate.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <functional>



namespace infrastructure {

template <class T>
Delegate<T>::Delegate(const std::string& name, const void* const instance, std::function<void(T&)> function):
myName(name),
myInstance(instance),
myFunction(function) {
}



template <class T>
void Delegate<T>::operator()(T& arg) const {
    myFunction(arg);
}



template <class T>
std::string Delegate<T>::name() const {
    return myName;
}



template <class T>
const void* Delegate<T>::instance() const {
    return myInstance;
}



template <typename T>
bool operator==(const Delegate<T>& lhs, const Delegate<T>& rhs) {
    return (lhs.name() == rhs.name()) && (lhs.instance() == rhs.instance());
}

}
