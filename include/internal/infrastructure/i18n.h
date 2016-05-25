// i18n.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 Róbert Čerňanský



#ifndef I18N_H
#define I18N_H



#ifdef ENABLE_NLS
#include <libintl.h>
#endif



namespace infrastructure {

#ifdef ENABLE_NLS

#define _(message) dgettext(PACKAGE, message)

#else

#define INIT_I18N

#define _(message) (message)

#endif // ENABLE_NLS

}



#endif // I18N_H
