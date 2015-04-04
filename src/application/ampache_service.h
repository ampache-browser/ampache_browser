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

using namespace std;
using namespace infrastructure;
using namespace domain;



namespace application {

class ReadyAlbumsEventArgs {

public:
    vector<unique_ptr<Album>> albums;
    int offset;
    int limit;

    ReadyAlbumsEventArgs(vector<unique_ptr<Album>>& albums_in, int offset_in, int limit_in):
    albums{move(albums_in)},
    offset{offset_in},
    limit{limit_in} { }
};



class AmpacheService: public QObject {
    Q_OBJECT

public:
    explicit AmpacheService(string url, string user, string password);

    ~AmpacheService();

    Event<bool> connected{};

    Event<ReadyAlbumsEventArgs> readyAlbums{};

    int numberOfAlbums() const;

    void requestAlbums(int offset, int limit);

    const vector<Artist*> retrieveArtists() const;

    const vector<Track*> retrieveTracks() const;

private slots:
    void onFinished();
    void onAlbumArtFinished();

private:
    struct {
        const string Handshake = "handshake";
        const string Ping = "ping";
        const string Albums = "albums";
        const string Artists = "artists";
    } Method;

    const string myUrl;
    const string myUser;
    const string myPassword;

    string myAuthToken = "";
    int myNumberOfAlbums = 0;
    QNetworkAccessManager* myNetworkAccessManager = nullptr;

    pair<int, int> myCurrentOffsetAndLimit{-1, -1};
    queue<pair<int, int>> myAlbumsRequests{};
    multimap<string, unique_ptr<Album>> myPendingAlbumArts{};
    vector<unique_ptr<Album>> myFinishedAlbumArts{};

    void connectToServer();
    void callMethod(string name, map<string, string> arguments) const;
    void processHandshake(QXmlStreamReader& xmlStreamReader);
    void processAlbums(QXmlStreamReader& xmlStreamReader);
    map<string, unique_ptr<Album>> createAlbums(QXmlStreamReader& xmlStreamReader) const;
    void fillAlbumArts(map<string, unique_ptr<Album>>& artUrlsToAlbumsMap);
    string assembleUrlBase() const;
    string parseMethodName(const string& methodCallUrl) const;
};

}



#endif // AMPACHESERVICE_H
