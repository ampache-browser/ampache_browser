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
#include <memory>

#include <QtCore/QAbstractListModel>

#include "domain/album.h"
#include "src/application/models/requests.h"



namespace application {

class AmpacheService;
class RequestGroup;



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
