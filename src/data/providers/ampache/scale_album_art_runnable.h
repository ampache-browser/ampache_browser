// scale_album_art_runnable.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef SCALEALBUMARTRUNNABLE_H
#define SCALEALBUMARTRUNNABLE_H



#include <string>

#include <QObject>
#include <QRunnable>
#include <QImage>



namespace data {

class AlbumData;
class ArtistData;
class TrackData;



/**
 * @brief Scales the given image to a predefined size.
 *
 * Intended to run asynchronously using the QT threading support.
 */
class ScaleAlbumArtRunnable: public QObject, public QRunnable {
    Q_OBJECT

signals:
    void finished(ScaleAlbumArtRunnable* scaleAlbumArtRunnable);

public:
    /**
     * @brief Constructor.
     *
     * @param id Identifier of the scaled image.
     * @param imageData The image which shall be scaled.
     */
    explicit ScaleAlbumArtRunnable(const std::string id, const QByteArray imageData);

    /**
     * @brief Gets the identifier of the scaled image (which was passed to the constructor).
     *
     * @return std::string.
     */
    std::string getId() const;

    /**
     * @brief Gets the scaled image.
     *
     * @return QImage
     */
    QImage getResult() const;

private:
    // arguments from the constructor
    const std::string myId;
    const QByteArray myImageData;

    // scaled image
    QImage myScaledAlbumArt;

    void run() override;
};

}



#endif // SCALEALBUMARTRUNNABLE_H
