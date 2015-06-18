// ampache_browser_main_window.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ampache_browser_main_window_H
#define ampache_browser_main_window_H



#include <QMainWindow>
#include <QListView>
#include <QTreeView>
#include <QAction>



namespace ui {

class AmpacheBrowserMainWindow: public QMainWindow {
   Q_OBJECT

public:
    explicit AmpacheBrowserMainWindow(QWidget* parent = 0);
    ~AmpacheBrowserMainWindow();

    QListView* albumsListView;
    QListView* artistsListView;
    QTreeView* tracksTreeView;

    QAction* playAction;
};

}



#endif // ampache_browser_main_window_H
