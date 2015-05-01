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
#include <QtGui/QImageReader>
#include <QtGui/QImage>
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

AmpacheService::AmpacheService(string url, string user, string password):
myUrl{url},
myUser{user},
myPassword{password},
myNetworkAccessManager{new QNetworkAccessManager{this}} {
    connectToServer();
}



AmpacheService::~AmpacheService() {
    delete(myNetworkAccessManager);
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
    QXmlStreamReader xmlStreamReader{networkReply};

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
        } else if (name == "albums") {
            myNumberOfAlbums = stoi(value);
        }
    }

    if (xmlStreamReader.hasError()) {
      // TODO: handle error
    }

    bool b = false;
    connected(b);
}



void AmpacheService::processAlbums(QXmlStreamReader& xmlStreamReader) {
    auto artUrlsToAlbumsMap = createAlbums(xmlStreamReader);
    readyAlbums(artUrlsToAlbumsMap);
}



vector<pair<string, unique_ptr<Album>>> AmpacheService::createAlbums(QXmlStreamReader& xmlStreamReader) const {
    vector<pair<string, unique_ptr<Album>>> artUrlsToAlbumsMap{};

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
                artUrlsToAlbumsMap.emplace_back(artUrl, unique_ptr<Album>{new Album{id, albumName, year}});
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
                year = 0;
                try {
                    year = stoi(value);
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

    return artUrlsToAlbumsMap;
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
