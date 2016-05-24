// qt_application.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef AMPACHE_BROWSER_QTAPPLICATION_H
#define AMPACHE_BROWSER_QTAPPLICATION_H



#include <memory>
#include "export.h"

class QWidget;

namespace application {
class QtApplicationInternal;
}



namespace ampache_browser {

class AmpacheBrowser;
class Settings;



/**
 * @brief Instantiates the application with Qt UI and provides public interfaces.
 */
class AMPACHE_BROWSER_EXPORT QtApplication {

public:
    /**
     * @brief Creates instance of the application.
     *
     * Constructs Ampache Browser and its dependencies.
     */
    explicit QtApplication();

    ~QtApplication();

    /**
     * @brief Gets instance of Ampache Browser.
     *
     * @warning run() method has to be called prior to usage of the returned AmpacheBrowser instance.
     */
    AmpacheBrowser& getAmpacheBrowser() const;

    /**
     * @brief Gets instance of application settings.
     */
    Settings& getSettings() const;

    /**
     * @brief Gets main window widget.
     *
     * @note Widget is not created until run() is called and is invalidated when finishRequest() is called.
     *
     * @return The main window widget or nullptr if not created yet.
     */
    QWidget* getMainWidget() const;

    /**
     * @brief Creates UI window, connects to the server and starts reading data.
     *
     * @warning The Settings instance returned by getSettings() has to be initialized (all settings have to be set)
     * prior calling this method.
     */
    void run();

    /**
     * @brief Request to finish/stop the application.
     *
     * This method should be used to stop the application gracefully.  It signals to terminate all asynchronous
     * operations and once they are terminated it calls the passed callback function.  The callback can delete
     * the instance then.
     */
    void finishRequest(std::function<void()> finishedCb);

private:
    std::unique_ptr<application::QtApplicationInternal> myQtApplicationInternal;
};

}



#endif // AMPACHE_BROWSER_QTAPPLICATION_H
