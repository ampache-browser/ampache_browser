// application.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <memory>
#include <functional>
#include "ampache_browser/settings.h"
#include "ampache_browser/ampache_browser.h"
#include "infrastructure/logging/logging.h"
#include "application/settings_internal.h"
#include "application/ampache_browser_app.h"
#include "application/application.h"

using namespace std;
using namespace ampache_browser;
using namespace infrastructure;



namespace application {

Application::Application() {
    auto settingsInternal = new SettingsInternal{};
    mySettings = new Settings{unique_ptr<SettingsInternal>{settingsInternal}};
    myAmpacheBrowserApp = new AmpacheBrowserApp{*settingsInternal};
    myAmpacheBrowser = new AmpacheBrowser{unique_ptr<AmpacheBrowserApp>{myAmpacheBrowserApp}};
}



Application::~Application() {
}



unique_ptr<AmpacheBrowser> Application::getAmpacheBrowser() const {
    return unique_ptr<AmpacheBrowser>{myAmpacheBrowser};
}



unique_ptr<Settings> Application::getSettings() const {
    return unique_ptr<Settings>{mySettings};
}



AmpacheBrowserApp& Application::getAmpacheBrowserApp() const {
    return *myAmpacheBrowserApp;
}



void Application::run() {
    SET_LOG_LEVEL(verbosityToLogLevel(mySettings->getInt(Settings::LOGGING_VERBOSITY)));
    myAmpacheBrowserApp->run();
}



void Application::requestTermination(function<void()> terminatedCb) {
    myAmpacheBrowserApp->requestTermination(terminatedCb);
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

