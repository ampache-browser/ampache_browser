// ui.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef UI_H
#define UI_H



#include <QtCore/QObject>
#include <QtGui/QMainWindow>
#include <QtGui/QStandardItemModel>
#include "src/ui/ampache_browser_main_window.h"
#include "infrastructure/event.h"

using namespace infrastructure;



namespace ui {

class Ui: QObject {
    Q_OBJECT

public:
    explicit Ui();

    ~Ui();

//     Event<SelectedItems> playTriggered;

    Event<bool> albumWindowRedraw{};

    void setAlbumModel(QAbstractItemModel& model);

    void setArtistModel(QAbstractItemModel& model);

private slots:
    void onPlayActionTriggered();

private:
    AmpacheBrowserMainWindow* myMainWindow;
};

}



#endif // UI_H
