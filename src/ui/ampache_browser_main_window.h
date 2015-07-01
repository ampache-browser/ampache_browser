// ampache_browser_main_window.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ampache_browser_main_window_H
#define ampache_browser_main_window_H



#include <QMainWindow>

class QListView;
class QTreeView;
class QAction;



namespace ui {

class AmpacheBrowserMainWindow: public QMainWindow {
   Q_OBJECT

public:
    explicit AmpacheBrowserMainWindow(QWidget* parent = 0);

    ~AmpacheBrowserMainWindow();

    QListView* albumsListView = nullptr;
    QListView* artistsListView = nullptr;
    QTreeView* tracksTreeView = nullptr;

    QAction* playAction = nullptr;
};

}



#endif // ampache_browser_main_window_H
