// delegate.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef DELEGATE_H
#define DELEGATE_H



#include <functional>



namespace infrastructure {

#define DELEGATE0(functionName) \
    Delegate<void>{#functionName, this, std::bind(functionName, this)}

#define DELEGATE1(functionName, ...) \
    Delegate<__VA_ARGS__>{#functionName, this, std::bind(functionName, this, std::placeholders::_1)}



template <class T>
class Delegate {

public:
    explicit Delegate(const std::string& name, const void* const instance, std::function<void(T&)> function);

    void operator()(T& arg) const;

    std::string name() const;

    const void* instance() const;

private:
    std::string myName;
    const void* myInstance;
    std::function<void(T&)> myFunction;
};



template <>
class Delegate<void> {

public:
    explicit Delegate(const std::string& name, const void* const instance, std::function<void()> function);

    void operator()() const;

    std::string name() const;

    const void* instance() const;

private:
    std::string myName;
    const void* myInstance;
    std::function<void()> myFunction;
};



template <typename T>
bool operator==(const Delegate<T>& lhs, const Delegate<T>& rhs);

}



#include "src/infrastructure/event/delegate.cc"



#endif // DELEGATE_H
