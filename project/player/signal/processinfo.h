#ifndef PROCESSINFO_H
#define PROCESSINFO_H

// processinfo.h
// 10/18/2011

#include <QString>

struct ProcessInfo
{
  ulong processId;
  QString processName;
  QString executablePath;

  //QString hash;
  //qint64 tokenId;

  ProcessInfo() : processId(0) { }
  bool isValid() const { return processId; }
};

#endif // PROCESSINFO_H
