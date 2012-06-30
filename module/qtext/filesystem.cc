// qtext/filesystem.cc
// 3/16/2012
#include "module/qtext/filesystem.h"
#include <QtCore/QFile>

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

// EOF
