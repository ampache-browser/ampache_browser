// ui.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <QtCore/QObject>
#include <QListView>
#include <QTreeView>
#include <QAction>
#include <QtGui/QStandardItemModel>
#include <QItemSelection>
#include "ampache_browser_main_window.h"
#include "ui/ui.h"



namespace ui {

Ui::Ui():
myMainWindow{new AmpacheBrowserMainWindow{}} {
    connect(myMainWindow->playAction, SIGNAL(triggered()), this, SLOT(onPlayActionTriggered()));
    myMainWindow->show();
}



void Ui::setArtistModel(QAbstractItemModel& model) {
    myMainWindow->artistsListView->setModel(&model);
    connect(myMainWindow->artistsListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
        this, SLOT(onArtistsSelectionModelSelectionChanged(QItemSelection, QItemSelection)));
}



void Ui::setAlbumModel(QAbstractItemModel& model) {
    myMainWindow->albumsListView->setModel(&model);
    connect(myMainWindow->albumsListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
        this, SLOT(onAlbumsSelectionModelSelectionChanged(QItemSelection, QItemSelection)));
}



void Ui::setTrackModel(QAbstractItemModel& model) {
    myMainWindow->tracksTreeView->setModel(&model);
}



void Ui::onPlayActionTriggered() {
    bool b = false;
    albumWindowRedraw(b);
}



void Ui::onArtistsSelectionModelSelectionChanged(const QItemSelection&, const QItemSelection&) {
    string artistId = "0";
    auto selectedRows = myMainWindow->artistsListView->selectionModel()->selectedRows(1);
    if (!selectedRows.isEmpty()) {
        auto hiddenColumnIndex = selectedRows.first();
        artistId = hiddenColumnIndex.data().toString().toStdString();
    }
    artistSelected(artistId);
}



void Ui::onAlbumsSelectionModelSelectionChanged(const QItemSelection&, const QItemSelection&) {
    string albumId = "0";
    auto selectedRows = myMainWindow->albumsListView->selectionModel()->selectedRows(1);
    if (!selectedRows.isEmpty()) {
        auto hiddenColumnIndex = selectedRows.first();
        albumId = hiddenColumnIndex.data().toString().toStdString();
    }
    albumSelected(albumId);
}

}
