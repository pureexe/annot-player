// qxdatetime.cc
// 7/24/2011

#include "qxdatetime.h"

QTX_BEGIN_NAMESPACE

QTime
qxTimeFromMsec(qint64 msecs)
{
  if (msecs <= 0)
    return QTime(0, 0, 0);

  qint64 secs = msecs/1000;
  qint64 mins = secs ? secs/60 : 0;
  qint64 hours = mins ? mins/60 : 0;

  int h = hours;
  int m = mins ? int(mins % 60) : 0;
  int s = secs ? int(secs % 60) : 0;
  int ms = msecs % 1000;

  return QTime(h, m, s, ms);
}

QTX_END_NAMESPACE
