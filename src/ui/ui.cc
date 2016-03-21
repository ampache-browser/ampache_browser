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
    connect(myMainWindow->tracksTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
        this, SLOT(onTracksSelectionModelSelectionChanged(QItemSelection, QItemSelection)));
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
    vector<string> artistIds;
    for (auto hiddenColumnIndex: getAristSelectedRows()) {
        artistIds.push_back(hiddenColumnIndex.data().toString().toStdString());
    }
    artistsSelected(artistIds);
    enableOrDisablePlayActions();
}



void Ui::onAlbumsSelectionModelSelectionChanged(const QItemSelection&, const QItemSelection&) {
    vector<string> albumIds;
    for (auto albumHiddenColumnIndex: getAlbumsSelectedRows()) {
        albumIds.push_back(albumHiddenColumnIndex.data().toString().toStdString());
    }
    vector<string> artistIds;
    for (auto artistHiddenColumnIndex: getAristSelectedRows()) {
        artistIds.push_back(artistHiddenColumnIndex.data().toString().toStdString());
    }
    auto albumAndArtistIds = make_pair(albumIds, artistIds);

    albumsSelected(albumAndArtistIds);
    enableOrDisablePlayActions();
}



void Ui::onTracksSelectionModelSelectionChanged(const QItemSelection&, const QItemSelection&) {
    enableOrDisablePlayActions();
}



void Ui::onSearchTextChanged(const QString& searchText) {
    auto stdSearchText = searchText.toStdString();
    searchTriggered(stdSearchText);
    enableOrDisablePlayActions();
}



void Ui::onSearchReturnPressed() {
    auto searchText = myMainWindow->searchLineEdit->text().toStdString();
    searchTriggered(searchText);
    enableOrDisablePlayActions();
}



SelectedItems Ui::getSelectedItems() const {
    vector<string> artistIds;
    for (auto hiddenArtistColumnIndex: getAristSelectedRows()) {
        artistIds.push_back(hiddenArtistColumnIndex.data().toString().toStdString());
    }
    vector<string> albumIds;
    for (auto hiddenAlbumColumnIndex: getAlbumsSelectedRows()) {
        albumIds.push_back(hiddenAlbumColumnIndex.data().toString().toStdString());
    }
    vector<string> trackIds;
    for (auto hiddenTrackColumnIndex: getTracksSelectedRows()) {
        trackIds.push_back(hiddenTrackColumnIndex.data().toString().toStdString());
    }

    return SelectedItems{artistIds, albumIds, trackIds};
}



void Ui::enableOrDisablePlayActions() {
    bool enabled = getAristSelectedRows().size() + getAlbumsSelectedRows().size() + getTracksSelectedRows().size() != 0;
    myMainWindow->playAction->setEnabled(enabled);
    myMainWindow->createPlaylistAction->setEnabled(enabled);
    myMainWindow->addToPlaylistAction->setEnabled(enabled);
}



QModelIndexList Ui::getAristSelectedRows() const {
    return myMainWindow->artistsListView->selectionModel()->selectedRows(1);
}



QModelIndexList Ui::getAlbumsSelectedRows() const {
    return myMainWindow->albumsListView->selectionModel()->selectedRows(1);
}



QModelIndexList Ui::getTracksSelectedRows() const {
    return myMainWindow->tracksTreeView->selectionModel()->selectedRows(3);
}

}
