// settings.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <memory>
#include <functional>
#include <string>
#include <utility>

#include "application/settings_internal.h"
#include "ampache_browser/settings.h"

using namespace application;



// SMELL: With this approach the key (variable) can be anything - it is not checked against defined constants in .h.
namespace ampache_browser {

const std::string Settings::USE_DEMO_SERVER = "use_demo_server";

const std::string Settings::SERVER_URL = "server_url";

const std::string Settings::USER_NAME = "user_name";

const std::string Settings::PASSWORD_HASH = "password_hash";

const std::string Settings::LOGGING_VERBOSITY = "logging_verbosity";

const std::string Settings::PROXY_HOST = "proxy_host";

const std::string Settings::PROXY_PORT = "proxy_port";

const std::string Settings::PROXY_USER = "proxy_user";

const std::string Settings::PROXY_PASSWORD = "proxy_password";



Settings::~Settings() {
}



void Settings::connectChanged(std::function<void()> callback) {
    mySettingsInternal->connectChanged(callback);
}



std::string Settings::getString(const std::string& key) const {
    return mySettingsInternal->getString(key);
}



void Settings::setString(const std::string& key, const std::string& value) {
    mySettingsInternal->setString(key, value);
}



int Settings::getInt(const std::string& key) const {
    return mySettingsInternal->getInt(key);
}



void Settings::setInt(const std::string& key, int value) {
    mySettingsInternal->setInt(key, value);
}



bool Settings::getBool(const std::string& key) const {
    return mySettingsInternal->getBool(key);
}



void Settings::setBool(const std::string& key, bool value) {
    mySettingsInternal->setBool(key, value);
}



void Settings::beginGroupSet() {
    mySettingsInternal->beginGroupSet();
}



void Settings::endGroupSet() {
    mySettingsInternal->endGroupSet();
}



Settings::Settings(
    std::unique_ptr<SettingsInternal> settingsInternal): mySettingsInternal(std::move(settingsInternal)) { }

}
