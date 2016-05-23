// settings_internal.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef SETTINGSINTERNAL_H
#define SETTINGSINTERNAL_H



#include <string>
#include <map>
#include <functional>



namespace application {

/**
 * @brief Internal implementation of the application settings container.
 */
class SettingsInternal {

public:
    /**
     * @brief Creates empty instance.
     */
    explicit SettingsInternal();

    /**
     * @brief Connect a callback function to a settings changed event.
     *
     * @sa beginGroupSet(), endGroupSet()
     *
     * @param callback Called after settings has changed.
     */
    void connectChanged(std::function<void()> callback);

    /**
     * @brief Gets the value of the given configuration variable of type string.
     *
     * @param key The name of configuration variable.
     */
    std::string getString(const std::string& key) const;

    /**
     * @brief Sets the value of the given configuration variable of type string.
     *
     * @param key The name of configuration variable.
     * @param value The value.
     */
    void setString(const std::string& key, const std::string& value);

    /**
     * @brief Gets the value of the given configuration variable of type int.
     *
     * @param key The name of configuration variable.
     */
    int getInt(const std::string& key) const;

    /**
     * @brief Sets the value of the given configuration variable of type int.
     *
     * @param key The name of configuration variable.
     * @param value The value.
     */
    void setInt(const std::string& key, int value);

    /**
     * @brief Gets the value of the given configuration variable of type bool.
     *
     * @param key The name of configuration variable.
     */
    bool getBool(const std::string& key) const;

    /**
     * @brief Sets the value of the given configuration variable of type bool.
     *
     * @param key The name of configuration variable.
     * @param value The value.
     */
    void setBool(const std::string& key, bool value);

    /**
     * @brief Denotes the start of a group of set* calls.
     *
     * Until endGroupSet() is called the callback set via connectChanged() will not be called.
     */
    void beginGroupSet();

    /**
     * @brief Denotes the end of a group of set* calls.
     *
     * The callback set via connectChanged() will be called if any of the set* calls that were made after
     * beginGroupSet() caused a change.
     */
    void endGroupSet();

private:
    std::map<std::string, std::string> myStrings;
    std::map<std::string, int> myInts;
    std::map<std::string, bool> myBools;
    std::function<void()> myChangedCb = []() { };

    bool myIsGroupSet = false;
    bool myWasChangedDuringGroupSet = false;

    void fireChanged();
};

}



#endif // SETTINGSINTERNAL_H
