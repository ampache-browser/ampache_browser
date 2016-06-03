// connection_info.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <string>
#include <QCryptographicHash>
#include "data/providers/connection_info.h"

using namespace std;



namespace data {

ConnectionInfo::ConnectionInfo(const std::string& serverUrl, const std::string& userName,
    const std::string& passwordHash, const string& proxyHost, const unsigned short proxyPort, const string& proxyUser,
    const string& proxyPassword):
myServerUrl(serverUrl),
myUserName(userName),
myPasswordHash{passwordHash},
myProxyHost(proxyHost),
myProxyPort(proxyPort),
myProxyUser(proxyUser),
myProxyPassword(proxyPassword) {
}



string ConnectionInfo::getServerUrl() const {
    return myServerUrl;
}



string ConnectionInfo::getUserName() const {
    return myUserName;
}



string ConnectionInfo::getPasswordHash() const {
    return myPasswordHash;
}



string ConnectionInfo::getProxyHost() const {
    return myProxyHost;
}



unsigned short ConnectionInfo::getProxyPort() const {
    return myProxyPort;
}



string ConnectionInfo::getProxyUser() const {
    return myProxyUser;
}



string ConnectionInfo::getProxyPassword() const {
    return myProxyPassword;
}



bool ConnectionInfo::isProxyUsed() const {
    return myProxyHost != "";
}



bool data::ConnectionInfo::doesProxyRequireAuthentication() const {
    return myProxyUser != "";
}

}
