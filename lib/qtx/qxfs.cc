// qxfs.cc
// 3/16/2012

#include "qtx/qxfs.h"
#include <QtCore/QFile>
#ifdef Q_OS_WIN
# include "qtwin/qtwin.h"
#endif // Q_OS_WIN
#ifdef WITH_MAC_QTCOCOA
# include "qtcocoa/qtcocoa.h"
#endif // WITH_MAC_QTCOCOA

QTX_BEGIN_NAMESPACE

QString
qxEscapeFileName(const QString &name)
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
qxTouchFile(const QString &fileName)
{ return QFile(fileName).open(QIODevice::WriteOnly); }

bool
qxTrashFile(const QString &fileName)
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
qxTrashOrRemoveFile(const QString &fileName)
{
  return QFile::exists(fileName) && (
    qxTrashFile(fileName) ||
    QFile::remove(fileName)
  );
}

QTX_END_NAMESPACE
