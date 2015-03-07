// ampache_browser_main_window.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <QtGui/QToolBar>
#include <QtGui/QStyle>
#include <QtGui/QAction>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QListView>
#include <QtGui/QLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QDockWidget>


#include "ui/ampache_browser_main_window.h"



namespace ui
{

AmpacheBrowserMainWindow::AmpacheBrowserMainWindow(QWidget *parent): QMainWindow(parent)
{
    // TODO: icon
    setWindowTitle(tr("Ampache Browser"));
    setMinimumSize(200, 200);
    setGeometry(40, 40, 850, 620);
    setDockOptions(AnimatedDocks);

    // tool bar
    auto playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    auto enqueueAction = new QAction(style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Enqueue"), this);
    auto searchLineEdit = new QLineEdit();
    auto spacerWidget = new QWidget();
    auto spacerWidget2 = new QWidget();
    auto mainToolBar = addToolBar(tr("Main"));
    auto aboutAction = new QAction(style()->standardIcon(QStyle::SP_ComputerIcon), tr("About"), this);

    searchLineEdit->setPlaceholderText(tr("Search..."));
    searchLineEdit->setToolTip(tr("Search for artists, albums and tracks."));
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
    auto albumsListView = new QListView();

    centralLayout->addWidget(albumsListView);
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    // docks
    auto artistsLayout = new QVBoxLayout();
    auto tracksLayout = new QHBoxLayout();
    auto artistsDockWidget = new QDockWidget(tr("Artists"),  this);
    auto tracksDockWidget = new QDockWidget(tr("Tracks"),  this);
    auto artistsListView = new QListView();
    auto tracksListView = new QListView();

    artistsDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    artistsDockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    artistsDockWidget->setLayout(artistsLayout);
    artistsDockWidget->setWidget(artistsListView);
    tracksDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    tracksDockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    tracksDockWidget->setLayout(tracksLayout);
    tracksDockWidget->setWidget(tracksListView);
    addDockWidget(Qt::RightDockWidgetArea, artistsDockWidget);
    addDockWidget(Qt::RightDockWidgetArea, tracksDockWidget);

    // status bar
    statusBar()->showMessage(tr("Ready"));
}

AmpacheBrowserMainWindow::~AmpacheBrowserMainWindow()
{}

};
