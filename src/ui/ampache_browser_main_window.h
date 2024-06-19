// ampache_browser_main_window.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#ifndef AMPACHEBROWSERMAINWINDOW_H
#define AMPACHEBROWSERMAINWINDOW_H



#include <QObject>
#include <QMainWindow>

class QListView;
class QTreeView;
class QLineEdit;
class QAction;
class QWidget;



namespace ui {

class CustomProxyStyle;
class SettingsDialog;



/**
 * @brief Application (plugin) main window.
 */
class AmpacheBrowserMainWindow: public QMainWindow {
   Q_OBJECT

public:
    explicit AmpacheBrowserMainWindow(QWidget* parent = 0);

    ~AmpacheBrowserMainWindow();

    CustomProxyStyle* myCustomProxyStyle = nullptr;

    QListView* albumsListView = nullptr;
    QListView* artistsListView = nullptr;
    QTreeView* tracksTreeView = nullptr;

    QLineEdit* searchLineEdit = nullptr;
    QAction* playAction = nullptr;
    QAction* createPlaylistAction = nullptr;
    QAction* addToPlaylistAction = nullptr;

    SettingsDialog* settingsDialog = nullptr;
};

}



#endif // AMPACHEBROWSERMAINWINDOW_H
