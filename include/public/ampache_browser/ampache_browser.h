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
#include "export.h"

class QWidget;

namespace application {
class AmpacheBrowserApp;
}



namespace ampache_browser {

class Settings;



/**
 * @brief The Ampache Browser application public interface.
 */
class AMPACHE_BROWSER_EXPORT AmpacheBrowser {

public:
    /**
     * @brief Creates the application instance.
     *
     * Creates UI window, connects to the server and starts reading data.
     *
     * @param settings The application user settings.
     */
    explicit AmpacheBrowser(Settings* const settings);

    /**
     * @brief Destroys the instance.
     *
     * @warning The requestTermination method should be called first and the instance destroyed after the callback
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
     * @brief Gets the main window widget of the application.
     */
    QWidget* getMainWidget() const;

    /**
     * @brief Request to terminate the application.
     *
     * This method should be used to end the application gracefully.  It signals to terminate all asynchronous
     * operations and once they are terminated it calls the passed callback function.  The callback can delete
     * the instance then.
     *
     * The implementation must ensure that no instance variable is accessed after the callback returns because
     * the instance will be already destroyed.
     */
    void requestTermination(std::function<void()> terminatedCb);

private:
    std::unique_ptr<application::AmpacheBrowserApp> myAmpacheBrowserApp;
};

}



#endif // AMPACHEBROWSER_H
