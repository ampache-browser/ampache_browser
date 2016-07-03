// album_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 -2016 Róbert Čerňanský



#ifndef ALBUMMODEL_H
#define ALBUMMODEL_H



#include <memory>
#include <QtCore/QAbstractListModel>
#include "src/application/models/requests.h"

namespace data {
class AlbumRepository;
}



namespace application {

class RequestGroup;



/**
 * @brief Provides albums data to a QT view.
 */
class AlbumModel: public QAbstractTableModel {
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     *
     * @param albumRepository Provides albums and means to trigger their loading from an external source.
     * @param parent
     */
    explicit AlbumModel(data::AlbumRepository* const albumRepository, QObject* parent = 0);

    virtual ~AlbumModel();

    /**
     * @sa QAbstractTableModel::data()
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @sa QAbstractTableModel::rowCount()
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @sa QAbstractTableModel::columnCount()
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    // stores album repository provided in the constuctor
    data::AlbumRepository* const myAlbumRepository = nullptr;

    // requests to load albums from an external source
    const std::unique_ptr<Requests> myAlbumRequests{new Requests{60}};

    // requests to load album arts from an external source
    const std::unique_ptr<Requests> myArtRequests{new Requests{3}};

    // normally arts are being loaded using filtered offsets; when a filter is set and all arts which were requested
    // via data() method were loaded the loading is switched to "unfiltered mode" where all remaining arts are
    // requested using unfiltered offsets
    mutable bool myIsInUnfilteredArtsLoadMode = false;

    void onReadyToExecuteAlbums(RequestGroup requestGroup);
    void onLoaded(std::pair<int, int> offsetAndLimit);
    void onReadyToExecuteArts(RequestGroup requestGroup);
    void onArtsLoaded(std::pair<int, int> offsetAndCount);
    void onDataSizeOrFilterChanged();
    void onProviderChanged();

    void requestAllData();
    void requestUnloadedArts();
};

}



#endif // ALBUMMODEL_H
