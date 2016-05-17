// application.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef APPLICATION_H
#define APPLICATION_H



#include <memory>

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
     */
    std::unique_ptr<ampache_browser::AmpacheBrowser> getAmpacheBrowser() const;

    /**
     * @brief Gets instance of application settings.
     */
    std::unique_ptr<ampache_browser::Settings> getSettings() const;

protected:
    /**
     * @brief Gets instace of Ampache Browser application.
     */
    AmpacheBrowserApp& getAmpacheBrowserApp() const;

private:
    ampache_browser::AmpacheBrowser* myAmpacheBrowser;
    ampache_browser::Settings* mySettings;
    AmpacheBrowserApp* myAmpacheBrowserApp;
};

}



#endif // APPLICATION_H
