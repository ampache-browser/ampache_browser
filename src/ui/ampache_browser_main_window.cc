// ampache_browser_main_window.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <memory>
#include <QToolBar>
#include <QStatusBar>
#include <QStyle>
#include <QAction>
#include <QLineEdit>
#include <QListView>
#include <QTreeView>
#include <QHBoxLayout>
#include <QDockWidget>
#include "infrastructure/i18n.h"
#include "settings_dialog.h"
#include "ampache_browser_main_window.h"

using namespace std;
using namespace infrastructure;



namespace ui {

AmpacheBrowserMainWindow::AmpacheBrowserMainWindow(QWidget* parent): QMainWindow(parent) {
    setWindowFlags(Qt::Widget);

    // tool bar
    playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), _("Play"), this);
    playAction->setEnabled(false);
    createPlaylistAction = new QAction(style()->standardIcon(QStyle::SP_FileIcon), _("Create Playlist"), this);
    createPlaylistAction->setEnabled(false);
    addToPlaylistAction = new QAction(style()->standardIcon(QStyle::SP_MediaSeekForward), _("Add to Playlist"),
        this);
    addToPlaylistAction->setEnabled(false);
    searchLineEdit = new QLineEdit();
    auto spacerWidget = new QWidget();
    auto spacerWidget2 = new QWidget();
    auto mainToolBar = addToolBar(_("Main"));
    auto settingsAction = new QAction(style()->standardIcon(QStyle::SP_ComputerIcon), _("Settings"), this);

    searchLineEdit->setPlaceholderText(_("Search..."));
    searchLineEdit->setToolTip(_("Search for artists, albums and tracks."));

    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    spacerWidget2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainToolBar->setMovable(false);
    mainToolBar->addAction(playAction);
    mainToolBar->addAction(createPlaylistAction);
    mainToolBar->addAction(addToPlaylistAction);
    mainToolBar->addWidget(spacerWidget);
    mainToolBar->addWidget(spacerWidget2);
    mainToolBar->addWidget(searchLineEdit);
    mainToolBar->addSeparator();
    mainToolBar->addAction(settingsAction);

    // central widget
    auto centralWidget = new QWidget{};
    auto centralLayout = new QHBoxLayout{};
    albumsListView = new QListView{};
    albumsListView->setViewMode(QListView::ViewMode::IconMode);
    albumsListView->setResizeMode(QListView::ResizeMode::Adjust);
    albumsListView->setWordWrap(true);
    albumsListView->setGridSize(QSize(128, 192));
    albumsListView->setIconSize(QSize(100, 100));
    albumsListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    albumsListView->setSelectionBehavior(QAbstractItemView::SelectRows);

    centralLayout->addWidget(albumsListView);
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    // docks
    auto artistsDockWidget = new QDockWidget(_("Artists"));
    auto tracksDockWidget = new QDockWidget(_("Tracks"));
    artistsListView = new QListView{};
    artistsListView->setResizeMode(QListView::ResizeMode::Adjust);
    artistsListView->setUniformItemSizes(true);
    artistsListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    artistsListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tracksTreeView = new QTreeView{};
    tracksTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tracksTreeView->setIndentation(0);

    artistsDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    artistsDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    artistsDockWidget->setWidget(artistsListView);
    tracksDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    tracksDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    tracksDockWidget->setWidget(tracksTreeView);
    addDockWidget(Qt::RightDockWidgetArea, artistsDockWidget);
    addDockWidget(Qt::RightDockWidgetArea, tracksDockWidget);

    statusBar()->setSizeGripEnabled(false);

    settingsDialog = new SettingsDialog{};
    connect(settingsAction, SIGNAL(triggered()), settingsDialog, SLOT(open()));
}



AmpacheBrowserMainWindow::~AmpacheBrowserMainWindow() {
    delete(settingsDialog);
    delete(tracksTreeView);
    delete(artistsListView);
    delete(albumsListView);
    delete(searchLineEdit);
    delete(addToPlaylistAction);
    delete(createPlaylistAction);
    delete(playAction);
}

}
