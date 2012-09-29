#ifndef QXDEBUG_H
#define QXDEBUG_H

// qxdebug.h
// 10/16/2011 jichi

#if defined(DEBUG) && defined(QT_CORE_LIB) && !defined(QX_NO_DEBUG)
# include <QtCore/QDebug>
# define DPRINT(...)   qDebug(QString("%1:%2:").arg(DEBUG).arg(__FUNCTION__), \
                                 __VA_ARGS__)
# define DOUT(_msg)    qDebug() << QString("%1:%2:").arg(DEBUG).arg(__FUNCTION__) \
                                 << _msg

#else
# define DPRINT(_dummy)        (void)0
# define DOUT(_dummy)          (void)0

//#ifdef _MSC_VER
//# pragma warning (disable:4390)     // C4390: empty controlled statement found: is this the intent?
//#endif // _MSC_VER

//#ifdef __GNUC__
//# pragma GCC diagnostic ignored "-Wempty-body" // empty body in an if or else statement
//#endif // __GNUC__

#endif // DEBUG

#endif // QXDEBUG_H
