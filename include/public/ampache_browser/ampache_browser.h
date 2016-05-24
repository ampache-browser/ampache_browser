// ampache_browser.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef AMPACHEBROWSER_H
#define AMPACHEBROWSER_H



#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "export.h"

namespace application {
class AmpacheBrowserApp;
class Application;
}



namespace ampache_browser {

/**
 * @brief The Ampache Browser application public interface.
 */
class AMPACHE_BROWSER_EXPORT AmpacheBrowser {

public:
    /**
     * @brief Destroys the instance.
     *
     * @warning The instance can be destroyed after QtApplication::finishRequest() has finished (its callback called).
     */
    ~AmpacheBrowser();

    /**
     * @brief Sets a callback that will be called after the user performed "play" action.
     *
     * @param callback Function that shall be set as callback.
     */
    void connectPlay(std::function<void(std::vector<std::string>)> callback);

    /**
     * @brief Sets a callback that will be called after the user performed "create playlist" action.
     *
     * @param callback Function that shall be set as callback.
     */
    void connectCreatePlaylist(std::function<void(std::vector<std::string>)> callback);

    /**
     * @brief Sets a callback that will be called after the user performed "add to playlist" action.
     *
     * @param callback Function that shall be set as callback.
     */
    void connectAddToPlaylist(std::function<void(std::vector<std::string>)> callback);

private:
    // Application needs constructor
    friend class application::Application;

    std::unique_ptr<application::AmpacheBrowserApp> myAmpacheBrowserApp;

    /**
     * @brief Creates the instance of main application class.
     */
    explicit AmpacheBrowser(std::unique_ptr<application::AmpacheBrowserApp> ampacheBrowserApp);
};

}



#endif // AMPACHEBROWSER_H
