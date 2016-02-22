// ampache_url.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include "ampache_url.h"



using namespace std;



namespace data {

AmpacheUrl::AmpacheUrl(const string& url):
myUrl{url} {
}



AmpacheUrl AmpacheUrl::createAlbumArtUrl(const string& albumId, const string& urlBase, const string& authToken) {
    ostringstream urlStream;
    urlStream << urlBase << "image.php?id=" << albumId << "&" << PARAM_AUTH << "=" << authToken;
    return AmpacheUrl(urlStream.str());
}



string AmpacheUrl::parseIdValue() const {
    return parseValue(PARAM_ID);
}



string AmpacheUrl::parseActionValue() const {
    return parseValue(PARAM_ACTION);
}



AmpacheUrl AmpacheUrl::replaceSsidValue(const string& newSsidValue) const {
    return replaceValue(PARAM_SSID, newSsidValue);
}



AmpacheUrl AmpacheUrl::replaceAuthValue(const string& newAuthValue) const {
    return replaceValue(PARAM_AUTH, newAuthValue);
}



string AmpacheUrl::str() const {
    return myUrl;
}



string AmpacheUrl::parseValue(const string& parameterName) const {
    auto valuePositionAndLength = findParameterValue(parameterName);
    return myUrl.substr(valuePositionAndLength.first, valuePositionAndLength.second);
}



AmpacheUrl AmpacheUrl::replaceValue(const string& parameterName, const string& newValue) const {
    auto newUrl = myUrl;
    auto valuePositionAndLength = findParameterValue(parameterName);
    if (valuePositionAndLength.second > 0) {
        newUrl.replace(valuePositionAndLength.first, valuePositionAndLength.second, newValue);
    }
    return AmpacheUrl{newUrl};
}



pair<string::size_type, string::size_type> AmpacheUrl::findParameterValue(const string& parameterName) const {
    auto paramsStart = myUrl.find("?");
    if (paramsStart != string::npos) {
        auto parameterNameStart = myUrl.find(parameterName + "=", paramsStart);
        if (parameterNameStart != string::npos) {
            auto parameterValueStart = parameterNameStart + parameterName.length() + 1;
            auto parameterValueEnd = myUrl.find("&", parameterValueStart);
            if (parameterValueEnd == string::npos) {
                parameterValueEnd = myUrl.length();
            }
            return make_pair(parameterValueStart, parameterValueEnd - parameterValueStart);
        }
    }
    return make_pair(0, 0);
}



const string AmpacheUrl::PARAM_SSID = "ssid";
const string AmpacheUrl::PARAM_AUTH = "auth";
const string AmpacheUrl::PARAM_ID = "id";
const string AmpacheUrl::PARAM_ACTION = "action";

}
