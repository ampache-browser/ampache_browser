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
    explicit SettingsInternal();

    void connectChanged(std::function<void()> callback);

    std::string getString(const std::string& key) const;

    void setString(const std::string& key, const std::string& value);

    bool getBool(const std::string& key) const;

    void setBool(const std::string& key, bool value);

    void beginGroupSet();

    void endGroupSet();

private:
    void fireChanged();

    std::map<std::string, std::string> myStrings;
    std::map<std::string, bool> myBools;
    std::function<void()> myChangedCb = []() { };

    bool myIsGroupSet = false;
    bool myWasChangedDuringGroupSet = false;
};

}



#endif // SETTINGSINTERNAL_H
