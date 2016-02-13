// album_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 -2016 Róbert Čerňanský



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
    explicit AlbumModel(data::AlbumRepository& albumRepository, QObject* parent = 0);

    virtual ~AlbumModel();

    /**
     * @brief Fired when loading albums from an external source has been aborted.
     *
     * @sa abortDataRequests()
     */
    infrastructure::Event<void> dataRequestsAborted{};

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

    /**
     * @brief Tell the model to request load of all albums from an external source.
     */
    void requestAllData();

    /**
     * @brief Tell the model to abort all pending requests for loading albums from an external source.
     *
     * The ::dataRequestsAborted event is fired after the requests has been aborted.  After calling this method the
     * model will not perform any further attempts to load albums.  The method is intended to support
     * the graceful termination of the application.
     */
    void abortDataRequests();

private:
    // stores album repository provided in the constuctor
    data::AlbumRepository& myAlbumRepository;

    // requests to load albums from an external source
    const std::unique_ptr<Requests> myAlbumRequests{new Requests};

    // requests to load album arts from an external source
    const std::unique_ptr<Requests> myArtRequests{new Requests{3}};

    // true if abortDataRequests() was called
    bool myDataRequestsAborted = false;

    void onReadyToExecuteAlbums(RequestGroup requestGroup);
    void onLoaded(std::pair<int, int> offsetAndLimit);
    void onReadyToExecuteArts(RequestGroup requestGroup);
    void onArtsLoaded(std::pair<int, int> offsetAndLimit);
    void onFilterChanged();
};

}



#endif // ALBUMMODEL_H
