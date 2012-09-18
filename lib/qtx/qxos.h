#ifndef QXOS_H
#define QXOS_H

// qtext/os.h
// 4/6/2012

#include "qtx/qtxglobal.h"
#include <QtCore/QString>

QString qxMktemp(const QString &suffix = QString(), bool deleteLater = true);

void qxSleep(uint msecs);

class QxSleepTimer
{
   bool stopped_;
public:
  void stop() { stopped_ = true; }

  void start(int secs)
  {
    stopped_ = false;
    while (secs-- > 0 && !stopped_)
      qxSleep(1000);
  }
};

#endif // QXOS
