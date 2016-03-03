// artist_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ARTISTMODEL_H
#define ARTISTMODEL_H



#include <vector>
#include <unordered_map>
#include <memory>

#include <QAbstractTableModel>

#include "domain/artist.h"
#include "data/artist_repository.h"
#include "src/application/models/requests.h"



namespace application {

/**
 * @brief Provides artists data to a QT view.
 */
class ArtistModel: public QAbstractTableModel {
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     *
     * @param artistRepository Provides artists and means to trigger their loading from an external source.
     * @param parent
     */
    explicit ArtistModel(data::ArtistRepository& artistRepository, QObject* parent = 0);

    virtual ~ArtistModel();

    /**
     * @brief Fired when loading artists from an external source has been aborted.
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
     * @brief Tell the model to request load of all artists from an external source.
     */
    void requestAllData();

    // TODO: Not needed?
    /**
     * @brief Tell the model to abort all pending requests for loading artists from an external source.
     *
     * The ::dataRequestsAborted event is fired after the requests has been aborted.  After calling this method the
     * model will not perform any further attempts to load artists.  The method is intended to support
     * the graceful termination of the application.
     */
    void abortDataRequests();

private:
    // stores artist repository provided in the constuctor
    data::ArtistRepository& myArtistRepository;

    // requests to load artists from an external source
    const std::unique_ptr<Requests> myRequests{new Requests{60}};

    // true if abortDataRequests() was called
    bool myDataRequestsAborted = false;

    void onReadyToExecute(RequestGroup requestGroup);
    void onLoaded(std::pair<int, int> offsetAndLimit);
    void onFilterChanged();
    void onLoadingDisabled();
    void onProviderChanged();
};

}



#endif // ARTISTMODEL_H
