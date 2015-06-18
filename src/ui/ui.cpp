// ui.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <QtCore/QObject>
#include <QMainWindow>
#include <QListView>
#include <QtGui/QStandardItemModel>
#include "ampache_browser_main_window.h"
#include "ui/ui.h"



namespace ui {

Ui::Ui() {
    myMainWindow = new AmpacheBrowserMainWindow{};
    connect(myMainWindow->playAction, SIGNAL(triggered()), this, SLOT(onPlayActionTriggered()));
    myMainWindow->show();
}



Ui::~Ui() {
    delete(myMainWindow);
}



void Ui::setAlbumModel(QAbstractItemModel& model) {
    myMainWindow->albumsListView->setModel(&model);
}



void Ui::setArtistModel(QAbstractItemModel& model) {
    myMainWindow->artistsListView->setModel(&model);
}



void Ui::setTrackModel(QAbstractItemModel& model) {
    myMainWindow->tracksTreeView->setModel(&model);
}



void Ui::onPlayActionTriggered() {
    bool b = false;
    albumWindowRedraw(b);
}

}