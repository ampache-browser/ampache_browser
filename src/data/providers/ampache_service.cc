// ampache_service.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <iostream>
#include <sstream>
#include <chrono>
#include <memory>
#include <utility>

#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QCoreApplication>
#include <QThreadPool>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QImageReader>
#include <QImage>
#include <QPixmap>
#include <QXmlStreamReader>
#include <QCryptographicHash>

#include <libaudcore/vfs_async.h>

#include "domain/artist.h"
#include "domain/album.h"
#include "domain/track.h"
#include "../data_objects/album_data.h"
#include "../data_objects/artist_data.h"
#include "../data_objects/track_data.h"
#include "ampache_url.h"
#include "data/providers/ampache_service.h"

using namespace std;
using namespace placeholders;
using namespace chrono;
using namespace infrastructure;
using namespace domain;



namespace data {

void onGetContentsCStyleWrapper(const char* url, const Index<char>& buffer, void* userData) {
    auto& callback = *reinterpret_cast<AmpacheService::OnGetContentsFunc*>(userData);
    callback(url, buffer);
}



AmpacheService::AmpacheService(const string& url, const string& user, const string& passwordHash):
myUrl{url},
myUser{user},
myPasswordHash{passwordHash},
myOnGetContentsFunc{bind(&AmpacheService::onGetContents, this, _1, _2)},
myOnAlbumArtFinishedFunc{bind(&AmpacheService::onAlbumArtFinished, this, _1, _2)} {
    connectToServer();
}



bool AmpacheService::getIsConnected() const {
    return myIsConnected;
}



system_clock::time_point AmpacheService::getLastUpdate() const {
    return myLastUpdate;
}



int AmpacheService::numberOfAlbums() const {
    return myNumberOfAlbums;
}



int AmpacheService::numberOfArtists() const {
    return myNumberOfArtists;
}



int AmpacheService::numberOfTracks() const {
    return myNumberOfTracks;
}



void AmpacheService::requestAlbums(int offset, int limit) {
    callMethod(Method.Albums, {{"offset", to_string(offset)}, {"limit", to_string(limit)}});
}



void AmpacheService::requestArtists(int offset, int limit) {
    callMethod(Method.Artists, {{"offset",  to_string(offset)}, {"limit", to_string(limit)}});
}



void AmpacheService::requestTracks(int offset, int limit) {
    callMethod(Method.Tracks, {{"offset", to_string(offset)}, {"limit", to_string(limit)}});
}



void AmpacheService::requestAlbumArts(const vector<string>& ids) {
    if (ids.empty() || !getIsConnected()) {
        auto emptyAlbumArts = map<string, QPixmap>{};
        readyAlbumArts(emptyAlbumArts);
        return;
    }

    for (auto id: ids) {
        myPendingAlbumArts.insert(id);
        vfs_async_file_get_contents(AmpacheUrl::createAlbumArtUrl(id, myUrl, myAuthToken).str().c_str(),
            onGetContentsCStyleWrapper, &myOnAlbumArtFinishedFunc);
    }
}



void AmpacheService::connectToServer() {
    auto currentTime = to_string(chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().
        time_since_epoch()).count());
    auto passphrase = QCryptographicHash::hash(
        QByteArray::fromStdString(currentTime) + QByteArray::fromStdString(myPasswordHash),
        QCryptographicHash::Sha256).toHex().toStdString();

    ostringstream urlStream;
    urlStream << assembleUrlBase() << Method.Handshake << "&auth=" << passphrase << "&timestamp=" << currentTime
      << "&version=350001&user=" << myUser;

    vfs_async_file_get_contents(urlStream.str().c_str(), onGetContentsCStyleWrapper, &myOnGetContentsFunc);
}



void AmpacheService::callMethod(const string& name, const map<string, string>& arguments) {
    ostringstream urlStream;
    urlStream << assembleUrlBase() << name << "&auth=" << myAuthToken;
    for (auto nameValuePair: arguments) {
        urlStream << "&" << nameValuePair.first << "=" << nameValuePair.second;
    }

    vfs_async_file_get_contents(urlStream.str().c_str(), onGetContentsCStyleWrapper, &myOnGetContentsFunc);
}



void AmpacheService::onGetContents(const char* url, const Index<char>& contentBuffer) {
    bool error = !contentBuffer.len();

    string content = string{contentBuffer.begin(), static_cast<size_t>(contentBuffer.len())};
    QXmlStreamReader xmlStreamReader{QString::fromStdString(content)};

    string methodName = AmpacheUrl{url}.parseActionValue();
    if (methodName == Method.Handshake) {
        processHandshake(xmlStreamReader, error);
    } else if (methodName == Method.Ping) {
//         processPing(xmlStreamReader);
    } else if (methodName == Method.Albums) {
        processAlbums(xmlStreamReader);
    } else if (methodName == Method.Artists) {
        processArtists(xmlStreamReader);
    } else if (methodName == Method.Tracks) {
        processTracks(xmlStreamReader);
    }
}



void AmpacheService::processHandshake(QXmlStreamReader& xmlStreamReader, bool error) {
    if (error) {
        myIsConnected = false;
        connected();
        return;
    }

    myIsConnected = true;

    QDateTime update{};
    QDateTime add{};
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

    connected();
}



void AmpacheService::processAlbums(QXmlStreamReader& xmlStreamReader) {
    auto albumsData = createAlbums(xmlStreamReader);

    // application can be terminated after readyAlbums event therefore there should be no access to instance variables
    // after it is fired
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

    string id = "";
    string albumName = "";
    int year = 0;
    int disk = 0;
    int tracks = 0;
    string artistId = "";
    while (!xmlStreamReader.atEnd()) {
        xmlStreamReader.readNext();
        xmlElement = xmlStreamReader.name().toString();

        if (xmlStreamReader.isEndElement()) {
            if (xmlElement == "album") {albumData.emplace_back(
                new AlbumData{id, artistId, tracks, unique_ptr<Album>{new Album{id, albumName, year, disk}}});
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
            }
        }
    }

    if (xmlStreamReader.hasError()) {
      // TODO: handle error
    }

    return albumData;
}



void AmpacheService::onAlbumArtFinished(const char* artUrl, const Index<char>& contentBuffer) {
    auto scaleAlbumArtRunnable = new ScaleAlbumArtRunnable(AmpacheUrl{artUrl}.parseIdValue(),
        QByteArray{contentBuffer.begin(), contentBuffer.len()});
    scaleAlbumArtRunnable->setAutoDelete(false);
    connect(scaleAlbumArtRunnable, SIGNAL(finished(ScaleAlbumArtRunnable*)), this,
        SLOT(onScaleAlbumArtRunnableFinished(ScaleAlbumArtRunnable*)));
    QThreadPool::globalInstance()->start(scaleAlbumArtRunnable);
}



void AmpacheService::onScaleAlbumArtRunnableFinished(ScaleAlbumArtRunnable* scaleAlbumArtRunnable) {
    auto albumId = *(myPendingAlbumArts.find(scaleAlbumArtRunnable->getId()));
    QPixmap art;
    art.convertFromImage(scaleAlbumArtRunnable->getResult());

    myFinishedAlbumArts.emplace(albumId, art);
    myPendingAlbumArts.erase(albumId);

    scaleAlbumArtRunnable->deleteLater();

    if (myPendingAlbumArts.empty()) {
        auto finishedAlbumArts = myFinishedAlbumArts;
        myFinishedAlbumArts.clear();

        // application can be terminated after readyAlbumArts event therefore there should be no access to instance
        // variables after it is fired
        readyAlbumArts(finishedAlbumArts);
    }
}



void AmpacheService::processArtists(QXmlStreamReader& xmlStreamReader) {
    auto artistsData = createArtists(xmlStreamReader);

    // application can be terminated after readyArtists event therefore there should be no access to instance variables
    // after it is fired
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



void AmpacheService::processTracks(QXmlStreamReader& xmlStreamReader) {
    auto tracksData = createTracks(xmlStreamReader);

    // application can be terminated after readyTracks event therefore there should be no access to instance variables
    // after it is fired
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



string AmpacheService::assembleUrlBase() const {
    return myUrl + "/server/xml.server.php?action=";
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
