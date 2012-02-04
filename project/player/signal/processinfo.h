#ifndef PROCESSINFO_H
#define PROCESSINFO_H

// processinfo.h
// 10/18/2011

#include <QString>
#include <QMetaType>

struct ProcessInfo
{
  ulong processId;
  QString processName;
  QString executablePath;

  ProcessInfo() : processId(0) { }
  bool isValid() const { return processId; }
};
Q_DECLARE_METATYPE(ProcessInfo)

#endif // PROCESSINFO_H
