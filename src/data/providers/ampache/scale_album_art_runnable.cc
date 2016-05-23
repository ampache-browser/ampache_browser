// scale_album_art_runnable.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <QObject>
#include <QImage>

#include "scale_album_art_runnable.h"

using namespace std;



namespace data {

ScaleAlbumArtRunnable::ScaleAlbumArtRunnable(const string id, const QByteArray imageData):
myId{id},
myImageData{imageData} { }



string ScaleAlbumArtRunnable::getId() const {
    return myId;
}



QImage ScaleAlbumArtRunnable::getResult() const {
    return myScaledAlbumArt;
}



void ScaleAlbumArtRunnable::run() {
    QImage art{};
    art.loadFromData(myImageData);
    // SMELL: Image size is specified here.
    myScaledAlbumArt = art.scaled(100, 100, Qt::AspectRatioMode::IgnoreAspectRatio,
        Qt::TransformationMode::SmoothTransformation);
    emit finished(this);
}

}
