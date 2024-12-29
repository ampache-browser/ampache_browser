// connection_info.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <string>

#include "data/providers/connection_info.h"



namespace data {

ConnectionInfo::ConnectionInfo(const std::string& serverUrl, const std::string& userName,
    const std::string& passwordHash, const std::string& proxyHost, const unsigned short proxyPort,
    const std::string& proxyUser, const std::string& proxyPassword):
myServerUrl(serverUrl),
myUserName(userName),
myPasswordHash{passwordHash},
myProxyHost(proxyHost),
myProxyPort(proxyPort),
myProxyUser(proxyUser),
myProxyPassword(proxyPassword) {
}



std::string ConnectionInfo::getServerUrl() const {
    return myServerUrl;
}



std::string ConnectionInfo::getUserName() const {
    return myUserName;
}



std::string ConnectionInfo::getPasswordHash() const {
    return myPasswordHash;
}



std::string ConnectionInfo::getProxyHost() const {
    return myProxyHost;
}



unsigned short ConnectionInfo::getProxyPort() const {
    return myProxyPort;
}



std::string ConnectionInfo::getProxyUser() const {
    return myProxyUser;
}



std::string ConnectionInfo::getProxyPassword() const {
    return myProxyPassword;
}



bool ConnectionInfo::isProxyUsed() const {
    return myProxyHost != "";
}



bool data::ConnectionInfo::doesProxyRequireAuthentication() const {
    return myProxyUser != "";
}

}
