// qt_application.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include <memory>
#include <functional>
#include <QWidget>
#include "application/qt_application_internal.h"
#include "ampache_browser/settings.h"
#include "ampache_browser/ampache_browser.h"
#include "ampache_browser/qt_application.h"

using namespace std;
using namespace application;



namespace ampache_browser {

QtApplication::QtApplication(): myQtApplicationInternal(new QtApplicationInternal{}) {
}



QtApplication::~QtApplication() {
}



AmpacheBrowser& QtApplication::getAmpacheBrowser() const {
    return myQtApplicationInternal->getAmpacheBrowser();
}



Settings& QtApplication::getSettings() const {
    return myQtApplicationInternal->getSettings();
}



QWidget* QtApplication::getMainWidget() const {
    return myQtApplicationInternal->getMainWidget();
}



void QtApplication::run() {
    myQtApplicationInternal->run();
}



void QtApplication::finishRequest(function<void()> finishedCb) {
    myQtApplicationInternal->finishRequest(finishedCb);
}

}
