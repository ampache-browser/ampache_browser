// ui.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <vector>
#include <QObject>
#include <QStatusBar>
#include <QListView>
#include <QTreeView>
#include <QAction>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QLineEdit>
#include <QCompleter>
#include "ampache_browser_main_window.h"
#include "ui/selected_items.h"
#include "ui/ui.h"

using namespace std;



namespace ui {

Ui::Ui():
myMainWindow{new AmpacheBrowserMainWindow{}} {
    connect(myMainWindow->playAction, SIGNAL(triggered()), this, SLOT(onPlayActionTriggered()));
    connect(myMainWindow->createPlaylistAction, SIGNAL(triggered()), this, SLOT(onCreatePlaylistActionTriggered()));
    connect(myMainWindow->addToPlaylistAction, SIGNAL(triggered()), this, SLOT(onAddToPlaylistActionTriggered()));
    connect(myMainWindow->artistsListView, SIGNAL(activated(QModelIndex)), this, SLOT(onActivated(QModelIndex)));
    connect(myMainWindow->albumsListView, SIGNAL(activated(QModelIndex)), this, SLOT(onActivated(QModelIndex)));
    connect(myMainWindow->tracksTreeView, SIGNAL(activated(QModelIndex)), this, SLOT(onActivated(QModelIndex)));

    connect(myMainWindow->searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onSearchTextChanged(QString)));
    connect(myMainWindow->searchLineEdit, SIGNAL(returnPressed()), this, SLOT(onSearchReturnPressed()));

    myMainWindow->show();
}



QWidget* Ui::getMainWidget() const {
    return myMainWindow;
}



void Ui::showNotification(const string& message) {
    myMainWindow->statusBar()->showMessage(QString::fromStdString(message));
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
    myMainWindow->tracksTreeView->hideColumn(3);
}



void Ui::onPlayActionTriggered() {
    auto selectedItems = getSelectedItems();
    playTriggered(selectedItems);
}



void Ui::onCreatePlaylistActionTriggered() {
    auto selectedItems = getSelectedItems();
    createPlaylistTriggered(selectedItems);
}



void Ui::onAddToPlaylistActionTriggered() {
    auto selectedItems = getSelectedItems();
    addToPlaylistTriggered(selectedItems);
}



void Ui::onActivated(const QModelIndex&) {
    auto selectedItems = getSelectedItems();
    playTriggered(selectedItems);
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



void Ui::onSearchTextChanged(const QString& searchText) {
    auto stdSearchText = searchText.toStdString();
    searchTriggered(stdSearchText);
}



void Ui::onSearchReturnPressed() {
    auto searchText = myMainWindow->searchLineEdit->text().toStdString();
    searchTriggered(searchText);
}



SelectedItems Ui::getSelectedItems() const {
    auto selectedArtistRows = myMainWindow->artistsListView->selectionModel()->selectedRows(1);
    vector<string> artistIds;
    for (auto hiddenArtistColumnIndex: selectedArtistRows) {
        artistIds.push_back(hiddenArtistColumnIndex.data().toString().toStdString());
    }

    auto selectedAlbumRows = myMainWindow->albumsListView->selectionModel()->selectedRows(1);
    vector<string> albumIds;
    for (auto hiddenAlbumColumnIndex: selectedAlbumRows) {
        albumIds.push_back(hiddenAlbumColumnIndex.data().toString().toStdString());
    }

    auto selectedTrackRows = myMainWindow->tracksTreeView->selectionModel()->selectedRows(3);
    vector<string> trackIds;
    for (auto hiddenTrackColumnIndex: selectedTrackRows) {
        trackIds.push_back(hiddenTrackColumnIndex.data().toString().toStdString());
    }

    return SelectedItems{artistIds, albumIds, trackIds};
}

}
