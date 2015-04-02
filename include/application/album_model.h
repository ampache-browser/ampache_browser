// album_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ALBUMMODEL_H
#define ALBUMMODEL_H



#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QtCore/QAbstractListModel>

#include "src/application/ampache_service.h"
#include "src/application/managed_ampache_service.h"
#include "src/application/request_tracker.h"
#include "domain/album.h"

using namespace domain;



namespace application {

class AlbumModel: public QAbstractListModel {
    Q_OBJECT

public:
    explicit AlbumModel(AmpacheService& ampacheService, QObject* parent = 0);

    ~AlbumModel() override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    AmpacheService& myAmpacheService;
    ManagedAmpacheService* myManagedAmpacheService = nullptr;
    unordered_map<int, unique_ptr<Album>> myAlbums{};
    RequestTracker* myRequestTracker = new RequestTracker{};

    void onReadyAlbums(ReadyAlbumsEventArgs& readyAlbumsEventArgs);
    void onProgressAlbums(ReadyAlbumsEventArgs& readyAlbumsEventArgs);
};

}



#endif // ALBUMMODEL_H
