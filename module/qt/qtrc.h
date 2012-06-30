#ifndef _QT_QTRC_H
#define _QT_QTRC_H

// qt/qtrc.h
// 4/6/2011
#include <QtGlobal>

#define QTRC_NULL       ""

#define QTRC_TR_NULL      QTRC_NULL
//#define QTRC_PREFIX_TR    ":/tr/"
#ifdef Q_WS_X11
# define QTRC_PREFIX_TR    QT_HOME "translations/"
#else
# define QTRC_PREFIX_TR    QCoreApplication::applicationDirPath() + "/translations/"
#endif // Q_WS_X11

// - Backgrounds -

#define QTRC_TR_EN      QTRC_NULL
#define QTRC_TR_JA      QTRC_PREFIX_TR "qt_ja.qm"
#define QTRC_TR_ZH_CN   QTRC_PREFIX_TR "qt_zh_CN.qm"
#define QTRC_TR_ZH_TW   QTRC_PREFIX_TR "qt_zh_TW.qm"

#endif // _QT_QTRC_H
