// ampache_browser_main_window.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <QToolBar>
#include <QStyle>
#include <QAction>
#include <QLineEdit>
#include <QSpacerItem>
#include <QListView>
#include <QTreeView>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QStatusBar>
#include <QDockWidget>
#include <QCompleter>
#include "ampache_browser_main_window.h"



namespace ui {

AmpacheBrowserMainWindow::AmpacheBrowserMainWindow(QWidget* parent): QMainWindow(parent) {
    // TODO: icon
    setWindowTitle(tr("Ampache Browser"));
    setMinimumSize(200, 200);
    setGeometry(5, 90, 850, 620);
    setDockOptions(AnimatedDocks);

    // tool bar
    playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    auto enqueueAction = new QAction(style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Enqueue"), this);
    searchLineEdit = new QLineEdit();
    auto spacerWidget = new QWidget();
    auto spacerWidget2 = new QWidget();
    auto mainToolBar = addToolBar(tr("Main"));
    auto aboutAction = new QAction(style()->standardIcon(QStyle::SP_ComputerIcon), tr("About"), this);

    searchLineEdit->setPlaceholderText(tr("Search..."));
    searchLineEdit->setToolTip(tr("Search for artists, albums and tracks."));

    auto searchCompleter = new QCompleter{};
    searchCompleter->setCompletionRole(Qt::DisplayRole);
    searchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    searchLineEdit->setCompleter(searchCompleter);

    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    spacerWidget2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainToolBar->setMovable(false);
    mainToolBar->addAction(playAction);
    mainToolBar->addAction(enqueueAction);
    mainToolBar->addWidget(spacerWidget);
    mainToolBar->addWidget(spacerWidget2);
    mainToolBar->addWidget(searchLineEdit);
    mainToolBar->addSeparator();
    mainToolBar->addAction(aboutAction);

    // central widget
    auto centralWidget = new QWidget();
    auto centralLayout = new QHBoxLayout();
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
    auto artistsDockWidget = new QDockWidget(tr("Artists"),  this);
    auto tracksDockWidget = new QDockWidget(tr("Tracks"),  this);
    artistsListView = new QListView();
    artistsListView->setResizeMode(QListView::ResizeMode::Adjust);
    artistsListView->setUniformItemSizes(true);
    artistsListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    artistsListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tracksTreeView = new QTreeView();

    artistsDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    artistsDockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    artistsDockWidget->setWidget(artistsListView);
    tracksDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    tracksDockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    tracksDockWidget->setWidget(tracksTreeView);
    addDockWidget(Qt::RightDockWidgetArea, artistsDockWidget);
    addDockWidget(Qt::RightDockWidgetArea, tracksDockWidget);

    // status bar
    statusBar()->showMessage(tr("Ready"));
}



AmpacheBrowserMainWindow::~AmpacheBrowserMainWindow() {
    delete(tracksTreeView);
    delete(artistsListView);
    delete(albumsListView);
    delete(playAction);
}

}
