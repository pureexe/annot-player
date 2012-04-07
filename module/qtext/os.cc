// qtext/os.cc
// 4/6/2012
#include "module/qtext/os.h"
#include "module/qtext/filedeleter.h"
#ifdef Q_OS_WIN
#  include <qt_windows.h>
#else
#  include <ctime>
#  include <unistd.h>
#endif // Q_OS_WIN
#include <cstdio>

#ifdef _MSC_VER
#  pragma warning (disable:4996)     // C4996: This function or variable may be unsafe. (tmpnam, getenv)
#endif // _MSC_VER

//#define DEBUG "os"
#include "module/debug/debug.h"
#include <QDebug>

#define _qs(_cstr)      QString::fromLocal8Bit(_cstr)

void
QtExt::sleep(uint ms)
{
  DOUT("enter: ms =" << ms);
#ifdef Q_OS_WIN
  Sleep(ms);
#else
  uint r = ms % 1000,
       s = ms / 1000;
  if (r) {
    timespec ts = { s, r * 1000 * 1000 };
    ::nanosleep(&ts, NULL);
  } else
    ::sleep(s);
#endif // Q_OS_WIN
  DOUT("exit");
}

QString
QtExt::mktemp(const QString &suffix, bool deleteLater)
{
  QString ret;
#ifdef Q_WS_WIN
  ret = _qs(::getenv("tmp"));
  if (ret.isEmpty())
    ret = _qs(::getenv("temp"));
#endif // Q_WS_WIN

  ret.append(_qs(::tmpnam(0)));
  if (!suffix.isEmpty())
    ret.append(suffix);

  if (deleteLater)
    FileDeleter::globalInstance()->deleteFileLater(ret);
  return ret;
}

// EOF
