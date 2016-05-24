// application.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef APPLICATION_H
#define APPLICATION_H



#include <memory>
#include "infrastructure/logging/log_level.h"

namespace ampache_browser {
class AmpacheBrowser;
class Settings;
}



namespace application {

class AmpacheBrowserApp;



/**
 * @brief Instantiates the application and provides main interfaces.
 *
 * @note This class is not responsible for lifecycle of instances it provides.
 */
class Application {

public:
    /**
     * @brief Creates instance of the application.
     *
     * Constructs Ampache Browser instance and its dependencies.
     */
    explicit Application();

    virtual ~Application();

    /**
     * @brief Gets instance of Ampache Browser.
     *
     * @warning run() method has to be called prior to usage of the returned AmpacheBrowser instance.
     */
    std::unique_ptr<ampache_browser::AmpacheBrowser> getAmpacheBrowser() const;

    /**
     * @brief Gets instance of application settings.
     */
    std::unique_ptr<ampache_browser::Settings> getSettings() const;

    /**
     * @brief Creates UI window, connects to the server and starts reading data.
     *
     * @warning The Settings instance returned by getSettings() has to be initialized (all settings have to be set)
     * prior calling this method.
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

protected:
    /**
     * @brief Gets instace of Ampache Browser application.
     */
    AmpacheBrowserApp& getAmpacheBrowserApp() const;

private:
    ampache_browser::AmpacheBrowser* myAmpacheBrowser;
    ampache_browser::Settings* mySettings;
    AmpacheBrowserApp* myAmpacheBrowserApp;

    static infrastructure::LogLevel verbosityToLogLevel(int verbosity);
};

}



#endif // APPLICATION_H
