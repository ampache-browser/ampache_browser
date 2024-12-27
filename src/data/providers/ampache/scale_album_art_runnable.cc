// scale_album_art_runnable.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <Qt>
#include <QImage>

#include "scale_album_art_runnable.h"

using namespace std;


namespace data {

ScaleAlbumArtRunnable::ScaleAlbumArtRunnable(const string id, const QByteArray imageData, int size):
myId{id},
myImageData{imageData},
mySize{size} { }



string ScaleAlbumArtRunnable::getId() const {
    return myId;
}



QImage ScaleAlbumArtRunnable::getResult() const {
    return myScaledAlbumArt;
}



void ScaleAlbumArtRunnable::run() {
    QImage art{};
    art.loadFromData(myImageData);
    myScaledAlbumArt = art.scaled(mySize, mySize, Qt::AspectRatioMode::IgnoreAspectRatio,
        Qt::TransformationMode::SmoothTransformation);
    emit finished(this);
}

}
