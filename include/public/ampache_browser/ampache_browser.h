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
     * @warning The requestTermination() method should be called first and the instance destroyed after the callback
     * terminatedCb is called.
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

    /**
     * @brief Creates UI window, connects to the server and starts reading data.
     */
    void run();

    /**
     * @brief Request to terminate the application.
     *
     * This method should be used to end the application gracefully.  It signals to terminate all asynchronous
     * operations and once they are terminated it calls the passed callback function.  The callback can delete
     * the instance then.
     */
    void requestTermination(std::function<void()> terminatedCb);

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
