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

using namespace std;
using namespace application;



// SMELL: With this approach the key (variable) can be anything - it is not checked against defined constants in .h.
namespace ampache_browser {

const string Settings::USE_DEMO_SERVER = "use_demo_server";

const string Settings::SERVER_URL = "server_url";

const string Settings::USER_NAME = "user_name";

const string Settings::PASSWORD_HASH = "password_hash";

const string Settings::LOGGING_VERBOSITY = "logging_verbosity";

const string Settings::PROXY_HOST = "proxy_host";

const string Settings::PROXY_PORT = "proxy_port";

const string Settings::PROXY_USER = "proxy_user";

const string Settings::PROXY_PASSWORD = "proxy_password";



Settings::~Settings() {
}



void Settings::connectChanged(function<void()> callback) {
    mySettingsInternal->connectChanged(callback);
}



string Settings::getString(const string& key) const {
    return mySettingsInternal->getString(key);
}



void Settings::setString(const string& key, const string& value) {
    mySettingsInternal->setString(key, value);
}



int Settings::getInt(const string& key) const {
    return mySettingsInternal->getInt(key);
}



void Settings::setInt(const string& key, int value) {
    mySettingsInternal->setInt(key, value);
}



bool Settings::getBool(const string& key) const {
    return mySettingsInternal->getBool(key);
}



void Settings::setBool(const string& key, bool value) {
    mySettingsInternal->setBool(key, value);
}



void Settings::beginGroupSet() {
    mySettingsInternal->beginGroupSet();
}



void Settings::endGroupSet() {
    mySettingsInternal->endGroupSet();
}



Settings::Settings(unique_ptr<SettingsInternal> settingsInternal): mySettingsInternal(move(settingsInternal)) {
}

}
