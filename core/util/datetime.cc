// core/datetime.cc
// 7/24/2011

#include "core/util/datetime.h"
#include <QtCore>

namespace Core {

  QTime msecs2time(qint64 msecs)
  {
    if (msecs <= 0)
      return QTime(0, 0, 0);

    qint64 secs = qRound64(msecs / 1000);
    qint64 mins = secs? qRound64(secs / 60) : 0;
    qint64 hours = mins? qRound64(mins / 60) : 0;

    int h = hours? static_cast<int>(hours) : 0;
    int m = mins? static_cast<int>(mins % 60) : 0;
    int s = secs? static_cast<int>(secs % 60) : 0;
    int ms = static_cast<int>(msecs % 1000);

    return QTime(h, m, s, ms);
  }

} // namespace Core

// EOF
