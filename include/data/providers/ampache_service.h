// ampache_service.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#ifndef AMPACHESERVICE_H
#define AMPACHESERVICE_H



#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <chrono>

#include <QtCore/QObject>
#include <QtCore/QRunnable>
#include <QImage>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <libaudcore/index.h>

#include "infrastructure/event/event.h"

class QXmlStreamReader;



namespace data {

class AlbumData;
class ArtistData;
class TrackData;



// SMELL: No need to expose this one.
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



// SMELL: It is not needed to expose entire service as public interface.  Only creation and initialization is needed in
// application level. - Split the interface?
class AmpacheService: public QObject {
    Q_OBJECT

public:
    typedef std::function<void (const char* url, const Index<char>& buffer)> OnGetContentsFunc;

    explicit AmpacheService(const std::string& url, const std::string& user, const std::string& passwordHash);

    AmpacheService(const AmpacheService& other) = delete;

    AmpacheService& operator=(const AmpacheService& other) = delete;

    infrastructure::Event<void> connected{};

    infrastructure::Event<std::vector<std::unique_ptr<AlbumData>>> readyAlbums{};

    infrastructure::Event<std::map<std::string, QPixmap>> readyAlbumArts{};

    infrastructure::Event<std::vector<std::unique_ptr<ArtistData>>> readyArtists{};

    infrastructure::Event<std::vector<std::unique_ptr<TrackData>>> readyTracks{};

    bool getIsConnected() const;

    std::chrono::system_clock::time_point getLastUpdate() const;

    int numberOfAlbums() const;

    int numberOfArtists() const;

    int numberOfTracks() const;

    void requestAlbums(int offset, int limit);

    void requestArtists(int offset, int limit);

    void requestTracks(int offset, int limit);

    void requestAlbumArts(const std::vector<std::string>& urls);

private slots:
    void onScaleAlbumArtRunnableFinished(ScaleAlbumArtRunnable* scaleAlbumArtRunnable);

private:
    struct {
        const std::string Handshake = "handshake";
        const std::string Ping = "ping";
        const std::string Albums = "albums";
        const std::string Artists = "artists";
        const std::string Tracks = "songs";
    } Method;

    const std::string myUrl;
    const std::string myUser;
    const std::string myPasswordHash;

    bool myIsConnected = false;
    std::string myAuthToken = "";
    std::chrono::system_clock::time_point myLastUpdate = std::chrono::system_clock::time_point::min();
    int myNumberOfAlbums = 0;
    int myNumberOfArtists = 0;
    int myNumberOfTracks = 0;

    std::set<std::string> myPendingAlbumArts;
    std::map<std::string, QPixmap> myFinishedAlbumArts;

    OnGetContentsFunc myOnGetContentsFunc;
    OnGetContentsFunc myOnAlbumArtFinishedFunc;

    void onGetContent(const char* url, const Index<char>& contentBuffer);
    void onAlbumArtFinished(const char* artUrl, const Index<char>& contentBuffer);

    void connectToServer();
    void callMethod(const std::string& name, const std::map<std::string, std::string>& arguments);
    void processHandshake(QXmlStreamReader& xmlStreamReader, bool error);
    void processAlbums(QXmlStreamReader& xmlStreamReader);
    std::vector<std::unique_ptr<AlbumData>> createAlbums(QXmlStreamReader& xmlStreamReader) const;
    void processArtists(QXmlStreamReader& xmlStreamReader);
    std::vector<std::unique_ptr<ArtistData>> createArtists(QXmlStreamReader& xmlStreamReader) const;
    void processTracks(QXmlStreamReader& xmlStreamReader);
    std::vector<std::unique_ptr<TrackData>> createTracks(QXmlStreamReader& xmlStreamReader) const;
    std::string assembleUrlBase() const;
    std::string parseMethodName(const std::string& methodCallUrl) const;
};

}



#endif // AMPACHESERVICE_H
