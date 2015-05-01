// album_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef ALBUMMODEL_H
#define ALBUMMODEL_H



class RequestGroup;

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QtCore/QAbstractListModel>

#include "domain/album.h"
#include "src/application/models/requests.h"
#include "src/application/ampache_service.h"



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
    std::unordered_map<int, std::pair<std::string, std::unique_ptr<domain::Album>>> myAlbums;
    Requests* const myAlbumRequests = new Requests;
    Requests* const myArtRequests = new Requests{3};

    void onReadyToExecuteAlbums(RequestGroup& requestGroup);
    void onReadyAlbums(std::vector<std::pair<std::string, std::unique_ptr<domain::Album>>>& artUrlsAndAlbums);
    void onReadyToExecuteArts(RequestGroup& requestGroup);
    void onReadyAlbumArts(std::map<std::string, QPixmap>& artUrlsAndArts);
};

}



#endif // ALBUMMODEL_H
