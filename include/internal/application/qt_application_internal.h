// qt_application_internal.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef QTAPPLICATIONINTERNAL_H
#define QTAPPLICATIONINTERNAL_H



#include "application.h"

class QWidget;



namespace application {

/**
 * @brief Instantiates the application with Qt UI.
 *
 * @sa Application
 */
class QtApplicationInternal: public Application {

public:
    /**
     * @brief Constructor.
     *
     * @sa Application::Application()
     */
    explicit QtApplicationInternal();

    /**
     * @brief Gets main window widget.
     *
     * @note Widget is not created until AmpacheBrowser::run() is called.
     *
     * @return The main window widget or nullptr if not created yet.
     */
    QWidget* getMainWidget() const;
};

}



#endif // QTAPPLICATIONINTERNAL_H
