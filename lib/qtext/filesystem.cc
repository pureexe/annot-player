// qtext/filesystem.cc
// 3/16/2012
#include "lib/qtext/filesystem.h"
#include <QtCore/QFile>
#ifdef Q_OS_WIN
# include "win/qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef WITH_MAC_QTCOCOA
# include "mac/qtcocoa/qtcocoa.h"
#endif // WITH_MAC_QTCOCOA

QString
QtExt::escapeFileName(const QString &name)
{
  return QString(name)
    .remove('"')
    .remove('\'')
#ifdef _MSC_VER
    .replace('/', "\xef\xbc\x8f")
#else
    .replace('/', "／")
#endif // _MSC_VER
    .replace('\\', '-')
    .replace('|', '-')
#ifdef _MSC_VER
    .replace(':', "\xef\xbc\x9a")
#elif !defined(Q_OS_MAC)
    .replace(':', "：")
#endif // _MSC_VER
#ifdef _MSC_VER
    .replace('?', "\xef\xbc\x9f")
#elif !defined(Q_OS_MAC)
    .replace('?', "？")
#endif // _MSC_VER
    .trimmed();
}

bool
QtExt::touchFile(const QString &fileName)
{ return QFile(fileName).open(QIODevice::WriteOnly); }

bool
QtExt::trashFile(const QString &fileName)
{
#ifdef Q_OS_WIN
  return QtWin::trashFile(fileName);
#elif defined(WITH_MAC_QTCOCOA)
  return QtCocoa::trashFile(fileName);
#else
  Q_UNUSED(fileName)
  return false;
#endif // Q_OS_WIN
}

bool
QtExt::trashOrRemoveFile(const QString &fileName)
{
  return QFile::exists(fileName) && (
    trashFile(fileName) ||
    QFile::remove(fileName)
  );
}

// EOF
