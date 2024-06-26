// application_qt.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include <memory>
#include <functional>

#include "application/application_qt_internal.h"
#include "ampache_browser/application_qt.h"

class QtWidget;

using namespace std;
using namespace application;



namespace ampache_browser {

class AmpacheBrowser;
class Settings;



ApplicationQt::ApplicationQt(): myApplicationQtInternal(new ApplicationQtInternal{}) {
}



ApplicationQt::~ApplicationQt() {
}



AmpacheBrowser& ApplicationQt::getAmpacheBrowser() const {
    return myApplicationQtInternal->getAmpacheBrowser();
}



Settings& ApplicationQt::getSettings() const {
    return myApplicationQtInternal->getSettings();
}



QWidget* ApplicationQt::getMainWidget() const {
    return myApplicationQtInternal->getMainWidget();
}



void ApplicationQt::setNetworkRequestFunction(const ApplicationQt::NetworkRequestFn& networkRequestFn) {
    myApplicationQtInternal->setNetworkRequestFunction(networkRequestFn);
}



void ApplicationQt::run() {
    myApplicationQtInternal->run();
}



void ApplicationQt::finishRequest(function<void()> finishedCb) {
    myApplicationQtInternal->finishRequest(finishedCb);
}

}
