// application.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <memory>
#include <functional>

#include "data/providers/ampache.h"
#include "ampache_browser/settings.h"
#include "ampache_browser/ampache_browser.h"
#include "infrastructure/logging/log_level.h"
#include "infrastructure/logging/logging.h"
#include "application/settings_internal.h"
#include "application/ampache_browser_app.h"
#include "application/application.h"

using namespace std;
using namespace ampache_browser;
using namespace infrastructure;
using namespace data;



namespace application {

Application::Application() {
    auto settingsInternal = new SettingsInternal{};
    mySettings = unique_ptr<Settings>{new Settings{unique_ptr<SettingsInternal>{settingsInternal}}};
    myAmpacheBrowserApp = new AmpacheBrowserApp{*settingsInternal};
    myAmpacheBrowser = unique_ptr<AmpacheBrowser>{
        new AmpacheBrowser{unique_ptr<AmpacheBrowserApp>{myAmpacheBrowserApp}}};
}



Application::~Application() {
}



AmpacheBrowser& Application::getAmpacheBrowser() const {
    return *myAmpacheBrowser;
}



Settings& Application::getSettings() const {
    return *mySettings;
}



AmpacheBrowserApp& Application::getAmpacheBrowserApp() const {
    return *myAmpacheBrowserApp;
}



void Application::setNetworkRequestFunction(const Ampache::NetworkRequestFn& networkRequestFn) {
    myAmpacheBrowserApp->setNetworkRequestFunction(networkRequestFn);
}



void Application::run() {
    SET_LOG_LEVEL(verbosityToLogLevel(mySettings->getInt(Settings::LOGGING_VERBOSITY)));
    myAmpacheBrowserApp->run();
}



void Application::finishRequest(function<void()> finishedCb) {
    myAmpacheBrowserApp->finishRequest(finishedCb);
}



LogLevel Application::verbosityToLogLevel(int verbosity) {
    switch (verbosity) {
        case 0: return LogLevel::None;
        case 1: return LogLevel::Error;
        case 2: return LogLevel::Warning;
        case 3: return LogLevel::Info;
        default: return LogLevel::Debug;
    }
}

}

