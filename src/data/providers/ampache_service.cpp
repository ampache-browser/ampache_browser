// ampache_service.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <iostream>
#include <sstream>
#include <chrono>
#include <memory>
#include <utility>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtCore/QCoreApplication>
#include <QtCore/QThreadPool>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtGui/QImageReader>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QXmlStreamReader>
#include <QCryptographicHash>

#include "infrastructure/event.h"
#include "domain/artist.h"
#include "domain/album.h"
#include "domain/track.h"
#include "../data_objects/album_data.h"
#include "../data_objects/artist_data.h"
#include "../data_objects/track_data.h"
#include "data/providers/ampache_service.h"

using namespace std;
using namespace chrono;
using namespace infrastructure;
using namespace domain;



namespace data {

AmpacheService::AmpacheService(string url, string user, string password):
myUrl{url},
myUser{user},
myPassword{password},
myNetworkAccessManager{new QNetworkAccessManager{this}} {
    connectToServer();
}



system_clock::time_point AmpacheService::getLastUpdate() const {
    return myLastUpdate;
}



int AmpacheService::numberOfAlbums() const {
    return myNumberOfAlbums;
}



void AmpacheService::requestAlbums(int offset, int limit) {
    callMethod(Method.Albums, {{"offset", to_string(offset)}, {"limit", to_string(limit)}});
}



void AmpacheService::requestAlbumArts(vector<string> urls) {
    for (auto artUrl: urls) {
        myPendingAlbumArts.insert(artUrl);
        QNetworkReply* networkReply = myNetworkAccessManager->get(QNetworkRequest(QUrl(QString::fromStdString(
            artUrl))));
        connect(networkReply, SIGNAL(finished()), this, SLOT(onAlbumArtFinished()));
    }
}



int AmpacheService::numberOfArtists() const {
    return myNumberOfArtists;
}



void AmpacheService::requestArtists(int offset, int limit) {
    callMethod(Method.Artists, {{"offset",  to_string(offset)}, {"limit", to_string(limit)}});
}



int AmpacheService::numberOfTracks() const {
    return myNumberOfTracks;
}



void AmpacheService::requestTracks(int offset, int limit) {
    callMethod(Method.Tracks, {{"offset", to_string(offset)}, {"limit", to_string(limit)}});
}



void AmpacheService::connectToServer() {
    auto currentTime = to_string(chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().
        time_since_epoch()).count());
    auto key = QCryptographicHash::hash(QByteArray::fromStdString(myPassword), QCryptographicHash::Sha256).toHex();
    auto passphrase = QCryptographicHash::hash(QByteArray::fromStdString(currentTime) + key,
        QCryptographicHash::Sha256).toHex().toStdString();

    ostringstream urlStream;
    urlStream << assembleUrlBase() << Method.Handshake << "&auth=" << passphrase << "&timestamp=" << currentTime
      << "&version=350001&user=" << myUser;

    QNetworkReply* networkReply = myNetworkAccessManager->get(QNetworkRequest(QUrl(QString::fromStdString(
        urlStream.str()))));
    connect(networkReply, SIGNAL(finished()), this, SLOT(onFinished()));
}



void AmpacheService::callMethod(string name, map<string, string> arguments) const {
    ostringstream urlStream;
    urlStream << assembleUrlBase() << name << "&auth=" << myAuthToken;
    for (auto nameValuePair: arguments) {
        urlStream << "&" << nameValuePair.first << "=" << nameValuePair.second;
    }
    QNetworkReply* networkReply = myNetworkAccessManager->get(QNetworkRequest(QUrl(QString::fromStdString(
        urlStream.str()))));
    connect(networkReply, SIGNAL(finished()), this, SLOT(onFinished()));
}



void AmpacheService::onFinished() {
    auto networkReply = qobject_cast<QNetworkReply*>(sender());
    QXmlStreamReader xmlStreamReader{networkReply};

    string methodName = parseMethodName(networkReply->request().url().toString().toStdString());
    if (methodName == Method.Handshake) {
        processHandshake(xmlStreamReader);
    } else if (methodName == Method.Ping) {
//         processPing(xmlStreamReader);
    } else if (methodName == Method.Albums) {
        processAlbums(xmlStreamReader);
    } else if (methodName == Method.Artists) {
        processArtists(xmlStreamReader);
    } else if (methodName == Method.Tracks) {
        processTracks(xmlStreamReader);
    }

    networkReply->deleteLater();
}



void AmpacheService::processHandshake(QXmlStreamReader& xmlStreamReader) {
    QDateTime update;
    QDateTime add;
    while (!xmlStreamReader.atEnd()) {
        xmlStreamReader.readNext();
        auto name = xmlStreamReader.name();
        if (!xmlStreamReader.isStartElement() || name == "root") {
            continue;
        }

        auto value = xmlStreamReader.readElementText().toStdString();
        if (name == "auth") {
            myAuthToken = value;
        } else if (name == "update") {
            update = QDateTime::fromString(QString::fromStdString(value), Qt::ISODate);
        } else if (name == "add") {
            add = QDateTime::fromString(QString::fromStdString(value), Qt::ISODate);
        } else if (name == "albums") {
            myNumberOfAlbums = stoi(value);
        } else if (name ==  "artists") {
            myNumberOfArtists = stoi(value);
        } else if (name ==  "songs") {
            myNumberOfTracks = stoi(value);
        }
    }
    myLastUpdate = system_clock::time_point{milliseconds{max(update, add).toMSecsSinceEpoch()}};

    if (xmlStreamReader.hasError()) {
      // TODO: handle error
    }

    bool b = false;
    connected(b);
}



void AmpacheService::processAlbums(QXmlStreamReader& xmlStreamReader) {
    auto albumsData = createAlbums(xmlStreamReader);
    readyAlbums(albumsData);
}



vector<unique_ptr<AlbumData>> AmpacheService::createAlbums(QXmlStreamReader& xmlStreamReader) const {
    vector<unique_ptr<AlbumData>> albumData{};

    QString xmlElement;
    while ((!xmlStreamReader.atEnd()) && (xmlElement != "root")) {
        xmlStreamReader.readNext();
        if (xmlStreamReader.isStartElement()) {
            xmlElement = xmlStreamReader.name().toString();
        }
    }

    string id;
    string albumName;
    int year;
    int tracks;
    string artUrl;
    string artistId;
    while (!xmlStreamReader.atEnd()) {
        xmlStreamReader.readNext();
        xmlElement = xmlStreamReader.name().toString();

        if (xmlStreamReader.isEndElement()) {
            if (xmlElement == "album") {albumData.emplace_back(
                new AlbumData{id, artUrl, artistId, tracks, unique_ptr<Album>{new Album{id, albumName, year}}});
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



void AmpacheService::onAlbumArtFinished() {
    auto networkReply = qobject_cast<QNetworkReply*>(sender());

    auto artUrl = networkReply->request().url().toString().toStdString();
    auto scaleAlbumArtRunnable = new ScaleAlbumArtRunnable(artUrl, networkReply->readAll());
    scaleAlbumArtRunnable->setAutoDelete(false);
    connect(scaleAlbumArtRunnable, SIGNAL(finished(ScaleAlbumArtRunnable*)), this,
        SLOT(onScaleAlbumArtRunnableFinished(ScaleAlbumArtRunnable*)));
    QThreadPool::globalInstance()->start(scaleAlbumArtRunnable);

    networkReply->deleteLater();
}



void AmpacheService::onScaleAlbumArtRunnableFinished(ScaleAlbumArtRunnable* scaleAlbumArtRunnable) {
    auto artUrl = *(myPendingAlbumArts.find(scaleAlbumArtRunnable->getId()));
    QPixmap art;
    art.convertFromImage(scaleAlbumArtRunnable->getResult());

    myFinishedAlbumArts.emplace(artUrl, art);
    myPendingAlbumArts.erase(artUrl);

    if (myPendingAlbumArts.empty()) {
        readyAlbumArts(myFinishedAlbumArts);
        myFinishedAlbumArts.clear();
    }

    scaleAlbumArtRunnable->deleteLater();
}



void AmpacheService::processArtists(QXmlStreamReader& xmlStreamReader) {
    auto artistsData = createArtists(xmlStreamReader);
    readyArtists(artistsData);
}



vector<unique_ptr<ArtistData>> AmpacheService::createArtists(QXmlStreamReader& xmlStreamReader) const {
    vector<unique_ptr<ArtistData>> artistsData{};

    QString xmlElement;
    while ((!xmlStreamReader.atEnd()) && (xmlElement != "root")) {
        xmlStreamReader.readNext();
        if (xmlStreamReader.isStartElement()) {
            xmlElement = xmlStreamReader.name().toString();
        }
    }

    string id;
    string artistName;
    int albums;
    int songs;
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



void AmpacheService::processTracks(QXmlStreamReader& xmlStreamReader) {
    auto tracksData = createTracks(xmlStreamReader);
    readyTracks(tracksData);
}



vector<unique_ptr<TrackData>> AmpacheService::createTracks(QXmlStreamReader& xmlStreamReader) const {
    vector<unique_ptr<TrackData>> tracksData{};

    QString xmlElement;
    while ((!xmlStreamReader.atEnd()) && (xmlElement != "root")) {
        xmlStreamReader.readNext();
        if (xmlStreamReader.isStartElement()) {
            xmlElement = xmlStreamReader.name().toString();
        }
    }

    string id;
    string title;
    int number;
    string url;
    string artistId;
    string albumId;
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



string AmpacheService::assembleUrlBase() const {
    return myUrl + "/server/xml.server.php?action=";
}



string AmpacheService::parseMethodName(const string& methodCallUrl) const {
    auto nameBeginPos = assembleUrlBase().length();
    auto nameEndPos = methodCallUrl.find("&", nameBeginPos);
    return methodCallUrl.substr(nameBeginPos, nameEndPos - nameBeginPos);
}



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
