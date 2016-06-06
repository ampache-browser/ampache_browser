// ampache.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <sstream>
#include <chrono>
#include <memory>
#include <utility>

#include <QObject>
#include <QString>
#include <QUrl>
#include <QDateTime>
#include <QThreadPool>
#include <QPixmap>
#include <QXmlStreamReader>
#include <QCryptographicHash>

#include "infrastructure/logging/logging.h"
#include "domain/artist.h"
#include "domain/album.h"
#include "domain/track.h"
#include "../../data_objects/album_data.h"
#include "../../data_objects/artist_data.h"
#include "../../data_objects/track_data.h"
#include "scale_album_art_runnable.h"
#include "data/providers/connection_info.h"
#include "ampache_url.h"
#include "data/providers/ampache.h"

using namespace std;
using namespace placeholders;
using namespace chrono;
using namespace infrastructure;
using namespace domain;



namespace data {

Ampache::Ampache(const ConnectionInfo& connectionInfo, const Ampache::NetworkRequestFn& networkRequestFn):
myConnectionInfo{connectionInfo},
myNetworkRequestFn{networkRequestFn},
myNetworkRequestCb{bind(&Ampache::onNetworkRequestFinished, this, _1, _2)},
myAlbumArtsNetworkRequestCb{bind(&Ampache::onAlbumArtsNetworkRequestFinished, this, _1, _2)} {
}



bool Ampache::getIsInitialized() const {
    return myIsInitialized;
}



string Ampache::getUrl() const {
    return myConnectionInfo.getServerUrl();
}



string Ampache::getUser() const {
    return myConnectionInfo.getUserName();
}



system_clock::time_point Ampache::getLastUpdate() const {
    return myLastUpdate;
}



int Ampache::numberOfAlbums() const {
    return myNumberOfAlbums;
}



int Ampache::numberOfArtists() const {
    return myNumberOfArtists;
}



int Ampache::numberOfTracks() const {
    return myNumberOfTracks;
}



void Ampache::initialize() {
    connectToServer();
}



void Ampache::requestAlbums(int offset, int limit) {
    callMethod(Method.Albums, {{"offset", to_string(offset)}, {"limit", to_string(limit)}});
}



void Ampache::requestArtists(int offset, int limit) {
    callMethod(Method.Artists, {{"offset",  to_string(offset)}, {"limit", to_string(limit)}});
}



void Ampache::requestTracks(int offset, int limit) {
    callMethod(Method.Tracks, {{"offset", to_string(offset)}, {"limit", to_string(limit)}});
}



void Ampache::requestAlbumArts(const map<string, string>& idsAndUrls) {
    if (idsAndUrls.empty() || !getIsInitialized()) {
        auto emptyAlbumArts = map<string, QPixmap>{};
        readyAlbumArts(emptyAlbumArts);
        return;
    }

    LOG_DBG("Getting %d album arts.", idsAndUrls.size());
    for (auto& idAndUrl: idsAndUrls) {
        myPendingAlbumArts.insert(idAndUrl.first);
        myNetworkRequestFn(idAndUrl.second, myAlbumArtsNetworkRequestCb);
    }
}



void Ampache::refreshSession() {
    myIsRefreshingSession = true;
    callMethod(Method.Ping, {{"auth", myAuthToken}});
}



string Ampache::refreshUrl(const string& url) const {
    if (getIsInitialized()) {
        // SMELL: We are replacing session ID value with authentication token, which is different, however it works.
        return AmpacheUrl{url}.replaceSsidValue(myAuthToken).replaceAuthValue(myAuthToken).str();
    }
    return url;
}



void Ampache::onNetworkRequestFinished(const string& url, const vector<char>& content) {
    auto qByteArrayContent = QByteArray{&content[0], static_cast<int>(content.size())};
    QXmlStreamReader errorXmlStreamReader{qByteArrayContent};
    bool error = isError(errorXmlStreamReader);

    QXmlStreamReader xmlStreamReader{qByteArrayContent};
    string methodName = AmpacheUrl{url}.parseActionValue();
    LOG_DBG("Server call of method '%s' has returned with content of length %d and error %d.",  methodName.c_str(),
        content.size(), error);
    dispatchToMethodHandler(methodName, xmlStreamReader, error);
}



void Ampache::onAlbumArtsNetworkRequestFinished(const string& artUrl, const vector<char>& content) {
    LOG_DBG("Album art request has returned with network content of length %d.", content.size());

    auto scaleAlbumArtRunnable = new ScaleAlbumArtRunnable(AmpacheUrl{artUrl}.parseIdValue(),
        QByteArray{&content[0], static_cast<int>(content.size())});
    scaleAlbumArtRunnable->setAutoDelete(false);
    connect(scaleAlbumArtRunnable, SIGNAL(finished(ScaleAlbumArtRunnable*)), this,
        SLOT(onScaleAlbumArtRunnableFinished(ScaleAlbumArtRunnable*)));
    QThreadPool::globalInstance()->start(scaleAlbumArtRunnable);
}



void Ampache::onScaleAlbumArtRunnableFinished(ScaleAlbumArtRunnable* scaleAlbumArtRunnable) {
    LOG_DBG("Scaling of album art with ID %s has returned.", scaleAlbumArtRunnable->getId().c_str());
    scaleAlbumArtRunnable->deleteLater();
    // SMELL: It crashes when not found.  Either use condition or do not search for ID at all and use the one from
    // scaleAlbumArtRunnable.
    auto albumId = *(myPendingAlbumArts.find(scaleAlbumArtRunnable->getId()));
    QPixmap art;
    art.convertFromImage(scaleAlbumArtRunnable->getResult());

    myFinishedAlbumArts.emplace(albumId, art);
    myPendingAlbumArts.erase(albumId);

    if (myPendingAlbumArts.empty()) {
        auto finishedAlbumArts = myFinishedAlbumArts;
        myFinishedAlbumArts.clear();

        readyAlbumArts(finishedAlbumArts);
    }
}



void Ampache::connectToServer() {
    LOG_DBG("Handshaking with server.");
    auto currentTime = to_string(chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().
        time_since_epoch()).count());
    auto passphrase = QCryptographicHash::hash(
        QByteArray{currentTime.c_str()} + QByteArray{myConnectionInfo.getPasswordHash().c_str()},
        QCryptographicHash::Sha256).toHex().data();

    ostringstream urlStream;
    urlStream << assembleUrlBase() << Method.Handshake << "&auth=" << passphrase << "&timestamp=" << currentTime
      << "&version=350001&user=" << myConnectionInfo.getUserName();

    myNetworkRequestFn(urlStream.str(), myNetworkRequestCb);
}



void Ampache::callMethod(const string& name, const map<string, string>& arguments) {
    if (!getIsInitialized()) {
        QXmlStreamReader xmlStreamReader;
        dispatchToMethodHandler(name, xmlStreamReader, true);
        return;
    }

    LOG_DBG("Calling server method '%s'.", name.c_str());
    ostringstream urlStream;
    urlStream << assembleUrlBase() << name << "&auth=" << myAuthToken;
    for (auto nameValuePair: arguments) {
        urlStream << "&" << nameValuePair.first << "=" << nameValuePair.second;
    }
    myNetworkRequestFn(urlStream.str(), myNetworkRequestCb);
}



bool Ampache::isError(QXmlStreamReader& xmlStreamReader) {
    bool error = false;
    while (!xmlStreamReader.atEnd()) {
        xmlStreamReader.readNext();
        auto xmlElement = xmlStreamReader.name();
        if (!xmlStreamReader.isStartElement() || xmlElement == "root") {
            continue;
        }

        if (xmlElement == "error") {
            error = true;
            break;
        }
    }
    if (xmlStreamReader.hasError()) {
        error = true;
    }
    return error;
}



void Ampache::dispatchToMethodHandler(const string& methodName, QXmlStreamReader& xmlStreamReader, bool error) {
    if (error) {
        myIsInitialized = false;
    }

    if (methodName == Method.Handshake && !myIsRefreshingSession) {
        processHandshake(xmlStreamReader, error);
    } else if (methodName == Method.Ping || (methodName == Method.Handshake && myIsRefreshingSession)) {
        processPing(xmlStreamReader,  error, methodName == Method.Handshake);
    } else if (methodName == Method.Albums) {
        processAlbums(xmlStreamReader, error);
    } else if (methodName == Method.Artists) {
        processArtists(xmlStreamReader, error);
    } else if (methodName == Method.Tracks) {
        processTracks(xmlStreamReader, error);
    }
}



void Ampache::processHandshake(QXmlStreamReader& xmlStreamReader, bool error) {
    if (!error) {
        myIsInitialized = true;
        readHandshakeData(xmlStreamReader);
    }

    initialized(error);
}



void Ampache::processPing(QXmlStreamReader& xmlStreamReader, bool error, bool isRetry) {
    if (error && !isRetry) {
        connectToServer();
        return;
    }

    if (!error) {
        myIsInitialized = true;
        if (isRetry) {
            readHandshakeData(xmlStreamReader);
        }
    }
    myIsRefreshingSession = false;
    readySession(error);
}



void Ampache::readHandshakeData(QXmlStreamReader& xmlStreamReader) {
    QDateTime update{};
    QDateTime add{};
    while (!xmlStreamReader.atEnd()) {
        xmlStreamReader.readNext();
        auto xmlElement = xmlStreamReader.name();
        if (!xmlStreamReader.isStartElement() || xmlElement == "root") {
            continue;
        }

        auto value = xmlStreamReader.readElementText().toStdString();
        if (xmlElement == "auth") {
            myAuthToken = value;
        } else if (xmlElement == "update") {
            update = QDateTime::fromString(QString::fromStdString(value), Qt::ISODate);
        } else if (xmlElement == "add") {
            add = QDateTime::fromString(QString::fromStdString(value), Qt::ISODate);
        } else if (xmlElement == "albums") {
            myNumberOfAlbums = stoi(value);
        } else if (xmlElement ==  "artists") {
            myNumberOfArtists = stoi(value);
        } else if (xmlElement ==  "songs") {
            myNumberOfTracks = stoi(value);
        }
    }
    myLastUpdate = system_clock::time_point{milliseconds{max(update, add).toMSecsSinceEpoch()}};

    if (xmlStreamReader.hasError()) {
      // TODO: handle error
    }
}



void Ampache::processAlbums(QXmlStreamReader& xmlStreamReader, bool error) {
    vector<unique_ptr<AlbumData>> albumsData{};
    if (!error) {
        albumsData = createAlbums(xmlStreamReader);
    }

    readyAlbums(albumsData);
}



vector<unique_ptr<AlbumData>> Ampache::createAlbums(QXmlStreamReader& xmlStreamReader) const {
    vector<unique_ptr<AlbumData>> albumData{};

    QString xmlElement;
    while ((!xmlStreamReader.atEnd()) && (xmlElement != "root")) {
        xmlStreamReader.readNext();
        if (xmlStreamReader.isStartElement()) {
            xmlElement = xmlStreamReader.name().toString();
        }
    }

    string id = "";
    string albumName = "";
    int year = 0;
    int disk = 0;
    int tracks = 0;
    string artUrl = "";
    string artistId = "";
    while (!xmlStreamReader.atEnd()) {
        xmlStreamReader.readNext();
        xmlElement = xmlStreamReader.name().toString();

        if (xmlStreamReader.isEndElement()) {
            if (xmlElement == "album") {albumData.emplace_back(
                new AlbumData{id, artUrl, artistId, tracks, unique_ptr<Album>{new Album{id, albumName, year, disk}}});
            }
        }

        if (!xmlStreamReader.isStartElement()) {
            continue;
        }

        if (xmlElement == "album") {
            QXmlStreamAttributes attributes = xmlStreamReader.attributes();
            if (attributes.hasAttribute("id")) {
                id = attributes.value("id").toString().toStdString();
            }
        } else if (xmlElement == "artist") {
            QXmlStreamAttributes attributes = xmlStreamReader.attributes();
            if (attributes.hasAttribute("id")) {
                artistId = attributes.value("id").toString().toStdString();
            }
        }
        else {
            auto value = xmlStreamReader.readElementText().toStdString();

            if (xmlElement == "name") {
                albumName = value;
            } else if (xmlElement == "year") {
                year = 0;
                try {
                    year = stoi(value);
                } catch (const invalid_argument& ex) {}
                catch (const out_of_range& ex) {}
            } else if (xmlElement == "disk") {
                disk = 0;
                try {
                    disk = stoi(value);
                } catch (const invalid_argument& ex) {}
                catch (const out_of_range& ex) {}
            } else if (xmlElement == "tracks") {
                tracks = 0;
                try {
                    tracks = stoi(value);
                } catch (const invalid_argument& ex) {}
                catch (const out_of_range& ex) {}
            } else if (xmlElement == "art") {
                artUrl = value;
            }
        }
    }

    if (xmlStreamReader.hasError()) {
      // TODO: handle error
    }

    return albumData;
}



void Ampache::processArtists(QXmlStreamReader& xmlStreamReader, bool error) {
    vector<unique_ptr<ArtistData>> artistsData{};
    if (!error) {
        artistsData = createArtists(xmlStreamReader);
    }

    readyArtists(artistsData);
}



vector<unique_ptr<ArtistData>> Ampache::createArtists(QXmlStreamReader& xmlStreamReader) const {
    vector<unique_ptr<ArtistData>> artistsData{};

    QString xmlElement;
    while ((!xmlStreamReader.atEnd()) && (xmlElement != "root")) {
        xmlStreamReader.readNext();
        if (xmlStreamReader.isStartElement()) {
            xmlElement = xmlStreamReader.name().toString();
        }
    }

    string id = "";
    string artistName = "";
    int albums = 0;
    int songs = 0;
    while (!xmlStreamReader.atEnd()) {
        xmlStreamReader.readNext();
        xmlElement = xmlStreamReader.name().toString();

        if (xmlStreamReader.isEndElement()) {
            if (xmlElement == "artist") {
                artistsData.emplace_back(new ArtistData{id, albums, songs,
                    unique_ptr<Artist>{new Artist{id, artistName}}});
            }
        }

        if (!xmlStreamReader.isStartElement()) {
            continue;
        }

        if (xmlElement == "artist") {
            QXmlStreamAttributes attributes = xmlStreamReader.attributes();
            if (attributes.hasAttribute("id")) {
                id = attributes.value("id").toString().toStdString();
            }
        }
        else {
            auto value = xmlStreamReader.readElementText().toStdString();

            if (xmlElement == "name") {
                artistName = value;
            } else if (xmlElement == "albums") {
                albums = 0;
                try {
                    albums = stoi(value);
                } catch (const invalid_argument& ex) {}
                catch (const out_of_range& ex) {}
            } else if (xmlElement == "songs") {
                songs = 0;
                try {
                    songs = stoi(value);
                } catch (const invalid_argument& ex) {}
                catch (const out_of_range& ex) {}
            }
        }
    }

    if (xmlStreamReader.hasError()) {
      // TODO: handle error
    }

    return artistsData;
}



void Ampache::processTracks(QXmlStreamReader& xmlStreamReader, bool error) {
    vector<unique_ptr<TrackData>> tracksData{};
    if (!error) {
        tracksData = createTracks(xmlStreamReader);
    }

    readyTracks(tracksData);
}



vector<unique_ptr<TrackData>> Ampache::createTracks(QXmlStreamReader& xmlStreamReader) const {
    vector<unique_ptr<TrackData>> tracksData{};

    QString xmlElement;
    while ((!xmlStreamReader.atEnd()) && (xmlElement != "root")) {
        xmlStreamReader.readNext();
        if (xmlStreamReader.isStartElement()) {
            xmlElement = xmlStreamReader.name().toString();
        }
    }

    string id = "";
    string title = "";
    int number = 0;
    string url = "";
    string artistId = "";
    string albumId = "";
    while (!xmlStreamReader.atEnd()) {
        xmlStreamReader.readNext();
        xmlElement = xmlStreamReader.name().toString();

        if (xmlStreamReader.isEndElement()) {
            if (xmlElement == "song") {
                tracksData.emplace_back(new TrackData{
                    id, artistId, albumId, unique_ptr<Track>{new Track{id, title, number, url}}});
            }
        }

        if (!xmlStreamReader.isStartElement()) {
            continue;
        }

        if (xmlElement == "song") {
            QXmlStreamAttributes attributes = xmlStreamReader.attributes();
            if (attributes.hasAttribute("id")) {
                id = attributes.value("id").toString().toStdString();
            }
        } else if (xmlElement == "artist") {
            QXmlStreamAttributes attributes = xmlStreamReader.attributes();
            if (attributes.hasAttribute("id")) {
                artistId = attributes.value("id").toString().toStdString();
            }
        } else if (xmlElement == "album") {
            QXmlStreamAttributes attributes = xmlStreamReader.attributes();
            if (attributes.hasAttribute("id")) {
                albumId = attributes.value("id").toString().toStdString();
            }
        }
        else {
            auto value = xmlStreamReader.readElementText().toStdString();

            if (xmlElement == "title") {
                title = value;
            } else if (xmlElement == "track") {
                number = 0;
                try {
                    number = stoi(value);
                } catch (const invalid_argument& ex) {}
                catch (const out_of_range& ex) {}
            } else if (xmlElement == "url") {
                url = value;
            }
        }
    }

    if (xmlStreamReader.hasError()) {
      // TODO: handle error
    }

    return tracksData;
}



string Ampache::assembleUrlBase() const {
    return myConnectionInfo.getServerUrl() + "/server/xml.server.php?action=";
}

}
