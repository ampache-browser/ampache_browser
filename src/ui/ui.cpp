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
    auto selectedRows = myMainWindow->artistsListView->selectionModel()->selectedRows(1);
    vector<string> artistIds;
    for (auto hiddenColumnIndex: selectedRows) {
        artistIds.push_back(hiddenColumnIndex.data().toString().toStdString());
    }
    artistsSelected(artistIds);
}



void Ui::onAlbumsSelectionModelSelectionChanged(const QItemSelection&, const QItemSelection&) {
    auto selectedRows = myMainWindow->albumsListView->selectionModel()->selectedRows(1);
    vector<string> albumIds;
    for (auto hiddenColumnIndex: selectedRows) {
        albumIds.push_back(hiddenColumnIndex.data().toString().toStdString());
    }
    albumsSelected(albumIds);
}

}
