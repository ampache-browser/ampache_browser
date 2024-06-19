// application_qt_internal.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2024 Róbert Čerňanský



#include "application/ampache_browser_app.h"
#include "application/application.h"
#include "application/application_qt_internal.h"

class QWidget;

using namespace std;
using namespace ampache_browser;



namespace application {

ApplicationQtInternal::ApplicationQtInternal(): Application() {
}



QWidget* ApplicationQtInternal::getMainWidget() const {
    return getAmpacheBrowserApp().getMainWidget();
}

}
