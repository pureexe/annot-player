#ifndef _QTEXT_DATETIME_H
#define _QTEXT_DATETIME_H

#include <QtCore/QDateTime>

// qtext/datetime.h
// 7/24/2011

namespace QtExt {

  // - Constructors -

  QTime msecs2time(qint64 msecs);

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

} // namespace QtExt

#endif // _QTEXT_DATETIME_H
