// ampache_service.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef AMPACHESERVICE_H
#define AMPACHESERVICE_H



#include <string>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <memory>
#include <functional>

#include <QtCore/QObject>
#include <QtCore/QRunnable>
#include <QtXml/QXmlStreamReader>
#include <QtNetwork/QNetworkReply>

#include "infrastructure/event.h"
#include "domain/album.h"
#include "domain/artist.h"
#include "domain/track.h"



namespace application {

class ScaleAlbumArtRunnable: public QObject, public QRunnable {
    Q_OBJECT

signals:
    void finished(ScaleAlbumArtRunnable* scaleAlbumArtRunnable);

public:
    explicit ScaleAlbumArtRunnable(const std::string id, const QByteArray imageData);

    std::string getId() const;

    QImage getResult() const;

private:
    const std::string myId;
    const QByteArray myImageData;
    QImage myScaledAlbumArt;

    void run() override;
};



class AmpacheService: public QObject {
    Q_OBJECT

public:
    explicit AmpacheService(std::string url, std::string user, std::string password);

    ~AmpacheService();

    infrastructure::Event<bool> connected{};

    infrastructure::Event<std::vector<std::pair<std::string, std::unique_ptr<domain::Album>>>> readyAlbums{};

    infrastructure::Event<std::map<std::string, QPixmap>> readyAlbumArts{};

    int numberOfAlbums() const;

    void requestAlbums(int offset, int limit);

    void requestAlbumArts(std::vector<std::string> urls);

    const std::vector<domain::Artist*> retrieveArtists() const;

    const std::vector<domain::Track*> retrieveTracks() const;

private slots:
    void onFinished();
    void onAlbumArtFinished();
    void onScaleAlbumArtRunnableFinished(ScaleAlbumArtRunnable* scaleAlbumArtRunnable);

private:
    struct {
        const std::string Handshake = "handshake";
        const std::string Ping = "ping";
        const std::string Albums = "albums";
        const std::string Artists = "artists";
    } Method;

    const std::string myUrl;
    const std::string myUser;
    const std::string myPassword;

    std::string myAuthToken = "";
    int myNumberOfAlbums = 0;
    QNetworkAccessManager* const myNetworkAccessManager = nullptr;

    std::set<std::string> myPendingAlbumArts;
    std::map<std::string, QPixmap> myFinishedAlbumArts;

    void connectToServer();
    void callMethod(std::string name, std::map<std::string, std::string> arguments) const;
    void processHandshake(QXmlStreamReader& xmlStreamReader);
    void processAlbums(QXmlStreamReader& xmlStreamReader);
    std::vector<std::pair<std::string, std::unique_ptr<domain::Album>>> createAlbums(
        QXmlStreamReader& xmlStreamReader) const;
    void fillAlbumArts(std::multimap<std::string, std::unique_ptr<domain::Album>>& artUrlsToAlbumsMap);
    std::string assembleUrlBase() const;
    std::string parseMethodName(const std::string& methodCallUrl) const;
};

}



#endif // AMPACHESERVICE_H
