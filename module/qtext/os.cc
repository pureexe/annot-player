// qtext/os.cc
// 4/6/2012
#include "module/qtext/os.h"
#include "module/qtext/filedeleter.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#ifdef Q_OS_WIN
# include <QtCore/QCoreApplication>
# include <qt_windows.h>
//# define TEMP_PATH     QCoreApplication::applicationDirPath() + "/" "tmp"
# define TEMP_PATH     QCoreApplication::applicationDirPath() // compromise liblua in luaresolver
#else
# include <ctime> // where nanosleep() and timespec is declared
# include <unistd.h>
#endif // Q_OS_WIN
#include <cstdio>

#ifdef _MSC_VER
# pragma warning (disable:4996)     // C4996: This function or variable may be unsafe. (tmpnam, getenv)
#endif // _MSC_VER

#define DEBUG "os"
#include "module/debug/debug.h"

#define _qs(_cstr)      QString::fromLocal8Bit(_cstr)

void
QtExt::sleep(uint ms)
{
  //DOUT("enter: ms =" << ms);
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
  //DOUT("exit");
}

QString
QtExt::mktemp(const QString &suffix, bool deleteLater)
{
  static bool ok = true;
  QString ret = _qs(::tmpnam(nullptr)) + suffix;
#ifdef Q_OS_WIN
  //ret = _qs(qgetenv("tmp"));
  //if (ret.isEmpty()) {
  //  ret = _qs(qgetenv("temp"));
  //  if (ret.isEmpty())
  //    ret = QDir::homePath();
  //}
  // Use application folder instead, in case %temp% is pointed to a readonly location.
  ret.prepend(TEMP_PATH);
#endif // Q_OS_WIN

  static bool once = true;
  if (once) {
    once = false;
    QDir dir = QFileInfo(ret).absoluteDir();
    if (!dir.exists())
      dir.mkpath(dir.absolutePath());
    if (!dir.exists()) {
      DOUT("WARNING: failed to access TMP folder");
      ok = false;
    }
  }

  if (!ok) {
    QString fileName = QFileInfo(ret).fileName();
    ret = QDir::homePath() + "/" + fileName;
  }

#ifdef Q_OS_WIN
  while (ret.endsWith('.'))
    ret.chop(1);
#endif // Q_OS_WIN

  if (deleteLater)
    FileDeleter::globalInstance()->deleteFileLater(ret);

  return ret;
}

// EOF
