// qtext/filesystem.cc
// 3/16/2012
#include "module/qtext/filesystem.h"

QString
QtExt::escapeFileName(const QString &name)
{
  QString ret = name;
  ret.remove('"');
  ret.remove('\'');
#ifdef _MSC_VER
  ret.replace('/', "\xef\xbc\x8f");
#else
  ret.replace('/', "／");
#endif // _MSC_VER
  ret.replace('\\', '-');
  ret.replace('|', '-');
#ifdef _MSC_VER
  ret.replace(':', "\xef\xbc\x9a");
#elif !defined Q_OS_MAC
  ret.replace(':', "：");
#endif // _MSC_VER
#ifdef _MSC_VER
  ret.replace('?', "\xef\xbc\x9f");
#elif !defined Q_OS_MAC
  ret.replace('?', "？");
#endif // _MSC_VER
  return ret.trimmed();
}

// EOF
