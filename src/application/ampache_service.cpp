// ampache_service.cpp
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#include <sstream>
#include <chrono>
#include <memory>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtGui/QImageReader>
#include <QtGui/QPixmap>
#include <QtXml/QXmlStreamReader>

#include <botan/sha2_32.h>
#include <botan/hex.h>

#include "infrastructure/event.h"
#include "domain/album.h"
#include "domain/artist.h"
#include "domain/track.h"
#include "ampache_service.h"

using namespace std;
using namespace infrastructure;
using namespace domain;



namespace application {

AmpacheService::AmpacheService(string url,  string user, string password):
myUrl{url},
myUser{user},
myPassword{password},
myNetworkAccessManager{new QNetworkAccessManager{this}} {
    connectToServer();
}



AmpacheService::~AmpacheService() {
    delete(myNetworkAccessManager);
}



void AmpacheService::requestAlbums(int offset, int limit) const {
    callMethod(Method.Albums, {{"offset", to_string(offset)}, {"limit", to_string(limit)}});
}



void AmpacheService::connectToServer() {
    Botan::SHA_256 sha256;
    auto currentTime = to_string(chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().
        time_since_epoch()).count());
    auto keyVector = sha256.process(myPassword);
    string key = Botan::hex_encode(keyVector, keyVector.size(), false);
    auto passphraseVector = sha256.process(currentTime + key);
    string passphrase = Botan::hex_encode(passphraseVector, passphraseVector.size(), false);

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
    QXmlStreamReader xmlStreamReader(networkReply);

    string methodName = parseMethodName(networkReply->request().url().toString().toStdString());
    if (methodName == Method.Handshake) {
        processHandshake(xmlStreamReader);
    } else if (methodName == Method.Ping) {
//         processPing(xmlStreamReader);
    } else if (methodName == Method.Albums) {
        processAlbums(xmlStreamReader);
    } else if (methodName == Method.Artists) {
//         processArtists(xmlStreamReader);
    }

    networkReply->deleteLater();
}



void AmpacheService::processHandshake(QXmlStreamReader& xmlStreamReader) {
    while (!xmlStreamReader.atEnd()) {
        xmlStreamReader.readNext();
        auto name = xmlStreamReader.name();
        if (!xmlStreamReader.isStartElement() || name == "root") {
            continue;
        }

        auto value = xmlStreamReader.readElementText().toStdString();
        if (name == "auth") {
            myAuthToken = value;
        }
    }

    if (xmlStreamReader.hasError()) {
      // TODO: handle error
    }
}



void AmpacheService::processAlbums(QXmlStreamReader& xmlStreamReader) {
    auto artUrlsToAlbumsMap = createAlbums(xmlStreamReader);
    fillAlbumArts(artUrlsToAlbumsMap);
}



map<string, unique_ptr<Album>> AmpacheService::createAlbums(QXmlStreamReader& xmlStreamReader) const {
    map<string, unique_ptr<Album>> artUrlsToAlbumsMap{};

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
    string artUrl;
    while (!xmlStreamReader.atEnd()) {
        xmlStreamReader.readNext();
        xmlElement = xmlStreamReader.name().toString();

        if (xmlStreamReader.isEndElement()) {
            if (xmlElement == "album") {
                artUrlsToAlbumsMap[artUrl] = unique_ptr<Album>{new Album{id, albumName, year}};
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
        }
        else {
            auto value = xmlStreamReader.readElementText().toStdString();

            if (xmlElement == "name") {
                albumName = value;
            } else if (xmlElement == "year") {
                year = stoi(value);
            } else if (xmlElement == "art") {
                artUrl = value;
            }
        }
    }

    if (xmlStreamReader.hasError()) {
      // TODO: handle error
    }

    return artUrlsToAlbumsMap;
}



void AmpacheService::fillAlbumArts(map<string, unique_ptr<Album>>& artUrlsToAlbumsMap) {
    for (auto& artUrlAndAlbumPair: artUrlsToAlbumsMap) {
        string artUrl = artUrlAndAlbumPair.first;

        QNetworkReply* networkReply = myNetworkAccessManager->get(QNetworkRequest(QUrl(QString::fromStdString(
            artUrl))));
        connect(networkReply, SIGNAL(finished()), this, SLOT(onAlbumArtFinished()));
        myPendingAlbumArts.insert(move(artUrlAndAlbumPair));
    }
}



void AmpacheService::onAlbumArtFinished() {
    auto networkReply = qobject_cast<QNetworkReply*>(sender());
    string url = networkReply->request().url().toString().toStdString();
    auto urlAndAlbum = myPendingAlbumArts.find(url);
    auto& album = urlAndAlbum->second;

    QPixmap art{};

    if (art.loadFromData(networkReply->readAll())) {
        album->setArt(new QPixmap{art.scaled(100, 100, Qt::AspectRatioMode::IgnoreAspectRatio,
            Qt::TransformationMode::SmoothTransformation)});
    } else {
        album->setArt(new QPixmap{100, 100});
    }

    myFinishedAlbumArts.push_back(move(album));
    myPendingAlbumArts.erase(urlAndAlbum);

    if (myPendingAlbumArts.empty()) {
        readyAlbums(myFinishedAlbumArts);
        myFinishedAlbumArts.clear();
    }

    networkReply->deleteLater();
}



string AmpacheService::assembleUrlBase() const {
    return myUrl + "/server/xml.server.php?action=";
}



string AmpacheService::parseMethodName(const string& methodCallUrl) const {
    auto nameBeginPos = assembleUrlBase().length();
    auto nameEndPos = methodCallUrl.find("&", nameBeginPos);
    return methodCallUrl.substr(nameBeginPos, nameEndPos - nameBeginPos);
}

}
