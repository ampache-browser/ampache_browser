// qt_application_internal.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2016 Róbert Čerňanský



#include "application/ampache_browser_app.h"
#include "application/application.h"
#include "application/qt_application_internal.h"

using namespace std;
using namespace ampache_browser;



namespace application {

QtApplicationInternal::QtApplicationInternal(): Application() {
}



QWidget* QtApplicationInternal::getMainWidget() const {
    return getAmpacheBrowserApp().getMainWidget();
}

}
