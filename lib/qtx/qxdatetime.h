#ifndef QXDATETIME_H
#define QXDATETIME_H

// qtext/datetime.h
// 7/24/2011

#include "qtx/qtxglobal.h"
#include <QtCore/QDateTime>

QTX_BEGIN_NAMESPACE

QTime qxTimeFromMsec(qint64 msecs);

namespace QxDateTime {

  // - Constructors -

  // - Time zone -

  enum TimeZone { JapanTimeZone = +9, ChinaTimeZone = +8 };

  inline QDateTime currentDateTimeInTimeZone(int hours)
  { return QDateTime::currentDateTimeUtc().addSecs(3600 * hours); }

  inline QDateTime currentJapanDateTime()
  { return currentDateTimeInTimeZone(JapanTimeZone); }
  inline QDateTime currentChinaDateTime()
  { return currentDateTimeInTimeZone(ChinaTimeZone); }

  inline int currentTimeZone() ///< DST is not considered
  { return QDateTime::currentDateTimeUtc().secsTo(QDateTime::currentDateTime()) / 3600; }

} // namespace QxDateTime

QTX_END_NAMESPACE

#endif // QXDATETIME_H
