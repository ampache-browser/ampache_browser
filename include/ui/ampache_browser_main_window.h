// ampache_browser_main_window.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ampache_browser_main_window_H
#define ampache_browser_main_window_H



#include <QtGui/QMainWindow>



namespace ui
{

class AmpacheBrowserMainWindow: public QMainWindow
{
   Q_OBJECT

public:
    explicit AmpacheBrowserMainWindow(QWidget *parent = 0);
    virtual ~AmpacheBrowserMainWindow();
};
};



#endif // ampache_browser_main_window_H
