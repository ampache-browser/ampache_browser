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
#include <map>
#include <queue>
#include <memory>
#include <functional>

#include <QtCore/QObject>
#include <QtXml/QXmlStreamReader>
#include <QtNetwork/QNetworkReply>

#include "infrastructure/event.h"
#include "domain/album.h"
#include "domain/artist.h"
#include "domain/track.h"



namespace application {

class AmpacheService: public QObject {
    Q_OBJECT

public:
    explicit AmpacheService(std::string url, std::string user, std::string password);

    ~AmpacheService();

    infrastructure::Event<bool> connected{};

    infrastructure::Event<std::vector<std::unique_ptr<domain::Album>>> readyAlbums{};

    int numberOfAlbums() const;

    void requestAlbums(int offset, int limit);

    const std::vector<Artist*> retrieveArtists() const;

    const std::vector<domain::Track*> retrieveTracks() const;

private slots:
    void onFinished();
    void onAlbumArtFinished();

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
    QNetworkAccessManager* myNetworkAccessManager = nullptr;

    std::multimap<std::string, std::unique_ptr<domain::Album>> myPendingAlbumArts{};
    std::vector<std::unique_ptr<domain::Album>> myFinishedAlbumArts{};

    void connectToServer();
    void callMethod(std::string name, std::map<std::string, std::string> arguments) const;
    void processHandshake(QXmlStreamReader& xmlStreamReader);
    void processAlbums(QXmlStreamReader& xmlStreamReader);
    std::map<std::string, std::unique_ptr<domain::Album>> createAlbums(QXmlStreamReader& xmlStreamReader) const;
    void fillAlbumArts(std::map<std::string, std::unique_ptr<domain::Album>>& artUrlsToAlbumsMap);
    std::string assembleUrlBase() const;
    std::string parseMethodName(const std::string& methodCallUrl) const;
};

}



#endif // AMPACHESERVICE_H
