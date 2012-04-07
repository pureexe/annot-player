#ifndef WEBBROWSER_CONFIG_H
#define WEBBROWSER_CONFIG_H

// webrowser_config.h
// 10/10/2011

#include <QtGlobal>

#ifdef WEBBROWSER_SHARED
#  define WB_API Q_DECL_EXPORT
#elif defined(WEBBROWSER_STATIC)
#  define WB_API
#else
#  define WB_API Q_DECL_IMPORT
#endif // WEBBROWSER_

#define WEBBROWSER_HOMEPAGE     "http://www.google.com"
#define WEBBROWSER_SEARCHENGINE "https://www.google.com/search?hl=ja&ie=UTF-8&q=%1"

#endif // WEBBROWSER_CONFIG_H
