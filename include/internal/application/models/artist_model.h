// artist_model.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef ARTISTMODEL_H
#define ARTISTMODEL_H



#include <memory>
#include <QAbstractTableModel>
#include "src/application/models/requests.h"

namespace data {
class ArtistRepository;
}



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
    explicit ArtistModel(data::ArtistRepository* const artistRepository, QObject* parent = 0);

    virtual ~ArtistModel();

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
    // stores artist repository provided in the constuctor
    data::ArtistRepository* const myArtistRepository = nullptr;

    // requests to load artists from an external source
    const std::unique_ptr<Requests> myRequests{new Requests{60}};

    void onReadyToExecute(RequestGroup requestGroup);
    void onLoaded(std::pair<int, int> offsetAndLimit);
    void onFilterChanged();
    void onProviderChanged();

    void requestAllData();
};

}



#endif // ARTISTMODEL_H
