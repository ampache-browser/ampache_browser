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



/**
 * @brief Simple delegate.
 */
template <typename T>
class Delegate {

public:
    /**
     * @brief Constructor.
     *
     * @param name Delegate name.
     * @param instance Class instance that contains the @p function.
     * @param function The function.
     */
    explicit Delegate(const std::string& name, const void* const instance, std::function<void(T&)> function);

    /**
     * @brief Calls the function passed to the constructor.
     *
     * @param arg Argument passed to the function.
     */
    void operator()(T& arg) const;

    /**
     * @brief Gets name of the delegate.
     *
     * @return std::string
     */
    std::string name() const;

    /**
     * @brief Gets the instance that contains the function passed to the constructor.
     *
     * @return const void*
     */
    const void* instance() const;

private:
    // arguments from the constructor
    std::string myName;
    const void* myInstance;
    std::function<void(T&)> myFunction;
};



/**
 * @brief Delegate specialization for function with no arguments.
 */
template <>
class Delegate<void> {

public:
    /**
     * @brief Constructor.
     *
     * @param name Delegate name.
     * @param instance Class instance that contains the @p function.
     * @param function The function.
     */
    explicit Delegate(const std::string& name, const void* const instance, std::function<void()> function);

    /**
     * @brief Calls the function passed to the constructor.
     */
    void operator()() const;

    /**
     * @brief Gets name of the delegate.
     *
     * @return std::string
     */
    std::string name() const;

    /**
     * @brief Gets the instance that contains the function passed to the constructor.
     *
     * @return const void*
     */
    const void* instance() const;

private:
    // arguments from the constructor
    std::string myName;
    const void* myInstance;
    std::function<void()> myFunction;
};



template <typename T>
bool operator==(const Delegate<T>& lhs, const Delegate<T>& rhs);

}



#include "src/infrastructure/event/delegate.cc"



#endif // DELEGATE_H
