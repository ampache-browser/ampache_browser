// custom_proxy_style.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2017 Róbert Čerňanský



#ifndef CUSTOMPROXYSTYLE_H
#define CUSTOMPROXYSTYLE_H



#include <QProxyStyle>



namespace ui {

/**
 * @brief Proxy style that customizes the default style behaviour.
 */
class CustomProxyStyle: public QProxyStyle {

public:
    int styleHint(StyleHint hint, const QStyleOption *option = 0, const QWidget *widget = 0,
        QStyleHintReturn *returnData = 0) const override;
};

}



#endif // CUSTOMPROXYSTYLE_H
