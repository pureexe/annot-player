#ifndef DEBUG_H
#define DEBUG_H

// module/debug/debug.h
// 10/16/2011

//#define DEBUG "debug"
//#define USE_MODE_DEBUG

#if defined(DEBUG) && defined(USE_MODE_DEBUG)
  #include <QDebug>
  //#define DOUT(_msg)    qDebug() << __FILE__ << _msg
  #define DOUT(_msg)    qDebug() << QString("%1:%2:").arg(DEBUG).arg(__FUNCTION__) << _msg
#else
  #define DOUT(_dummy)  (void)0

  //#ifdef _MSC_VER
  //  #pragma warning (disable:4390)     // C4390: empty controlled statement found: is this the intent?
  //#endif // _MSC_VER

  //#ifdef __GNUC__
  //  #pragma GCC diagnostic ignored "-Wempty-body" // empty body in an if or else statement
  //#endif // __GNUC__

#endif // DEBUG

#endif // DEBUG_H
