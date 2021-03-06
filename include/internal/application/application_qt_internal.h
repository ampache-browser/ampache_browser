// application_qt_internal.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef APPLICATIONQTINTERNAL_H
#define APPLICATIONQTINTERNAL_H



#include "application.h"

class QWidget;



namespace application {

/**
 * @brief Instantiates the application with Qt UI.
 *
 * @sa Application
 */
class ApplicationQtInternal: public Application {

public:
    /**
     * @brief Constructor.
     *
     * @sa Application::Application()
     */
    explicit ApplicationQtInternal();

    /**
     * @brief Gets main window widget.
     *
     * @note Widget is not created until run() is called and is invalidated when finishRequest() is called.
     *
     * @return The main window widget or nullptr if not created yet.
     */
    QWidget* getMainWidget() const;
};

}



#endif // APPLICATIONQTINTERNAL_H
