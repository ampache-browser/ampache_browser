// custom_proxy_style.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2017 Róbert Čerňanský



#include "custom_proxy_style.h"



namespace ui {

int CustomProxyStyle::styleHint(StyleHint hint, const QStyleOption* option, const QWidget* widget,
    QStyleHintReturn* returnData) const {

    if (hint == QStyle::SH_ItemView_ActivateItemOnSingleClick) {
        return 0;
    }
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

}
