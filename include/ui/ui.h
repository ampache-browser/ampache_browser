// ui.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef UI_H
#define UI_H



#include <memory>
#include <QObject>
#include "src/ui/ampache_browser_main_window.h"
#include "infrastructure/event.h"

class QAbstractItemModel;



namespace ui {

class Ui: QObject {
    Q_OBJECT

public:
    explicit Ui();

    Ui(const Ui& other) = delete;

    Ui& operator=(const Ui& other) = delete;

//     Event<SelectedItems> playTriggered;

    infrastructure::Event<bool> albumWindowRedraw{};

    infrastructure::Event<std::string> artistSelected{};

    void setAlbumModel(QAbstractItemModel& model);

    void setArtistModel(QAbstractItemModel& model);

    void setTrackModel(QAbstractItemModel& model);

private slots:
    void onPlayActionTriggered();
    void onArtistsListViewClicked(const QModelIndex& index);

private:
    const std::unique_ptr<AmpacheBrowserMainWindow> myMainWindow;
};

}



#endif // UI_H
