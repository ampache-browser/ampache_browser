// settings.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef SETTINGS_H
#define SETTINGS_H



#include <string>
#include <memory>
#include <functional>

namespace application {
class SettingsInternal;
}



namespace ampache_browser {

/**
 * @brief The application settings container.
 */
class Settings {

public:
    /**
     * @brief Configuration variable name for demo server usage.
     *
     * Value type: bool.
     */
    static const std::string USE_DEMO_SERVER;

    /**
     * @brief Configuration variable name for the server URL.
     *
     * Value type: string.
     */
    static const std::string SERVER_URL;

    /**
     * @brief Configuration variable name for user name.
     *
     * Value type: string.
     */
    static const std::string USER_NAME;

    /**
     * @brief Configuration variable name for password hash.
     *
     * Value type: string.
     */
    static const std::string PASSWORD_HASH;

    /**
     * @brief Creates empty instance.
     */
    explicit Settings();

    ~Settings();

    /**
     * @brief Connect a callback function to a settings changed event.
     *
     * @sa beginGroupSet, endGroupSet
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
     * Until endGroupSet is called the callback set via connectChanged will not be called.
     */
    void beginGroupSet();

    /**
     * @brief Denotes the end of a group of set* calls.
     *
     * The callback set via connectChanged will be called if any of the set* calls that were made after beginGroupSet
     * caused a change.
     */
    void endGroupSet();

private:

    // AmpacheBrowser needs mySettingsInternal in order to create AmpacheBrowserApp instance
    friend class AmpacheBrowser;

    std::unique_ptr<application::SettingsInternal> mySettingsInternal;
};

}



#endif // SETTINGS_H
