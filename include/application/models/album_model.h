// album_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ALBUMMODEL_H
#define ALBUMMODEL_H



#include <vector>
#include <memory>

#include <QtCore/QAbstractListModel>

#include "domain/album.h"
#include "data/album_repository.h"
#include "src/application/models/requests.h"



namespace application {

class RequestGroup;



class AlbumModel: public QAbstractTableModel {
    Q_OBJECT

public:
    explicit AlbumModel(data::AlbumRepository& albumRepository, QObject* parent = 0);

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    data::AlbumRepository& myAlbumRepository;
    const std::unique_ptr<Requests> myAlbumRequests{new Requests};
    const std::unique_ptr<Requests> myArtRequests{new Requests{3}};

    void onReadyToExecuteAlbums(RequestGroup& requestGroup);
    void onLoaded(std::pair<int, int>& offsetAndLimit);
    void onReadyToExecuteArts(RequestGroup& requestGroup);
    void onArtsLoaded(std::pair<int, int>& offsetAndLimit);
    void onFilterChanged();
};

}



#endif // ALBUMMODEL_H
