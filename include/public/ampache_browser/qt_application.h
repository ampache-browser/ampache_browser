// qt_application.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef QTAPPLICATION_H
#define QTAPPLICATION_H



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
 *
 * @note This class is not responsible for lifecycle of instances it provides.
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
     * @warning Settings instance returned by getSettings() has to be initialized (all settings have to be set) prior
     * to usage of the returned AmpacheBrowser instance.
     */
    std::unique_ptr<AmpacheBrowser> getAmpacheBrowser() const;

    /**
     * @brief Gets instance of application settings.
     */
    std::unique_ptr<Settings> getSettings() const;

    /**
     * @brief Gets main window widget.
     *
     * @note Widget is not created until AmpacheBrowser::run() is called.
     *
     * @return The main window widget or nullptr if not created yet.
     */
    QWidget* getMainWidget() const;

private:
    std::unique_ptr<application::QtApplicationInternal> myQtApplicationInternal;
};

}



#endif // QTAPPLICATION_H
