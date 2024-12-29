// ampache_url.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include "ampache_url.h"



namespace data {

AmpacheUrl::AmpacheUrl(const std::string& url):
myUrl{url} {
}



std::string AmpacheUrl::parseIdValue() const {
    return parseValue(PARAM_ID);
}



std::string AmpacheUrl::parseFilterValue() const {
    return parseValue(PARAM_FILTER);
}



std::string AmpacheUrl::parseActionValue() const {
    return parseValue(PARAM_ACTION);
}



AmpacheUrl AmpacheUrl::replaceSsidValue(const std::string& newSsidValue) const {
    return replaceValue(PARAM_SSID, newSsidValue);
}



AmpacheUrl AmpacheUrl::replaceAuthValue(const std::string& newAuthValue) const {
    return replaceValue(PARAM_AUTH, newAuthValue);
}



std::string AmpacheUrl::str() const {
    return myUrl;
}



std::string AmpacheUrl::parseValue(const std::string& parameterName) const {
    auto valuePositionAndLength = findParameterValue(parameterName);
    return myUrl.substr(valuePositionAndLength.first, valuePositionAndLength.second);
}



AmpacheUrl AmpacheUrl::replaceValue(const std::string& parameterName, const std::string& newValue) const {
    auto newUrl = myUrl;
    auto valuePositionAndLength = findParameterValue(parameterName);
    if (valuePositionAndLength.second > 0) {
        newUrl.replace(valuePositionAndLength.first, valuePositionAndLength.second, newValue);
    }
    return AmpacheUrl{newUrl};
}



std::pair<std::string::size_type, std::string::size_type> AmpacheUrl::findParameterValue(
    const std::string& parameterName) const {

    auto paramsStart = myUrl.find("?");
    if (paramsStart != std::string::npos) {
        auto parameterNameStart = myUrl.find(parameterName + "=", paramsStart);
        if (parameterNameStart != std::string::npos) {
            auto parameterValueStart = parameterNameStart + parameterName.length() + 1;
            auto parameterValueEnd = myUrl.find("&", parameterValueStart);
            if (parameterValueEnd == std::string::npos) {
                parameterValueEnd = myUrl.length();
            }
            return std::make_pair(parameterValueStart, parameterValueEnd - parameterValueStart);
        }
    }
    return std::make_pair(0, 0);
}



const std::string AmpacheUrl::PARAM_SSID = "ssid";
const std::string AmpacheUrl::PARAM_AUTH = "auth";
const std::string AmpacheUrl::PARAM_ID = "id";
const std::string AmpacheUrl::PARAM_FILTER = "filter";
const std::string AmpacheUrl::PARAM_ACTION = "action";

}
