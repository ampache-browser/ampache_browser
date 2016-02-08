// ampache_browser_main_window.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ampache_browser_main_window_H
#define ampache_browser_main_window_H



#include <QMainWindow>

class QListView;
class QTreeView;
class QLineEdit;
class QAction;



namespace ui {

/**
 * @brief Application (plugin) main window.
 */
class AmpacheBrowserMainWindow: public QMainWindow {
   Q_OBJECT

public:
    explicit AmpacheBrowserMainWindow(QWidget* parent = 0);

    ~AmpacheBrowserMainWindow();

    QListView* albumsListView = nullptr;
    QListView* artistsListView = nullptr;
    QTreeView* tracksTreeView = nullptr;

    QLineEdit* searchLineEdit = nullptr;
    QAction* playAction = nullptr;
};

}



#endif // ampache_browser_main_window_H
