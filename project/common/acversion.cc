// acversion.cc
// 8/20/2012
#include "project/common/acversion.h"
#include <QtCore/QStringList>

#define AC_VERSION  VERSION

int
AcVersion::currentVersionNumber()
{
  static int ret = 0;
  if (!ret)
    ret = toNumber(currentVersion());
  return ret;
}

// 0.1.2.3
int
AcVersion::toNumber(const QString &version)
{
  enum { Radix = 30 };
  if (version.isEmpty())
    return 0;
  int ret = 0;
  QStringList l = version.split('.', QString::SkipEmptyParts);
  switch (l.size()) {
  case 4: ret += l[3].toInt();
  case 3: ret += l[2].toInt() * Radix;
  case 2: ret += l[1].toInt() * Radix * Radix;
  //case 1: ret += l[0].toInt() * Radix * Radix * Radix;
  //case 0:
  //default:
  }
  return ret;
}

// EOF
