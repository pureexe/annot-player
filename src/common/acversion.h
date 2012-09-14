#ifndef ACVERSION_H
#define ACVERSION_H

// acversion.h
// 8/20/2012
#include <QtGlobal>

class AcVersion
{
  AcVersion() { }

public:
  static const char *currentVersion() { return VERSION; }
  static int currentVersionNumber();

  static int toNumber(const QString &version);

  static int compare(int v1, int v2)
  { return v1 - v2; }

  static int compare(const QString &v1, const QString &v2)
  { return toNumber(v1) - toNumber(v2); }
};

#endif // ACVERSION_H
