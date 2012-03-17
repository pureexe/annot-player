#ifndef _QTEXT_STRING_H
#define _QTEXT_STRING_H

// qtext/string.h
// 3/11/2012

#include <QString>

namespace QtExt {

  int matchLastString(const QString str, QRegExp &rx);

  QString increaseString(const QString &s);
  QString decreaseString(const QString &s);

} // namespace QtExt

#endif // _QTEXT_STRING_H
