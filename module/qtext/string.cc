// string.cc
// 3/11/2012

#include "module/qtext/string.h"
#include <QtCore>

int
QtExt::matchLastString(const QString str, QRegExp &rx)
{
  int pos = 0, ret = -1;
  while ((pos = rx.indexIn(str, pos)) >= 0) {
    ret = pos;
    pos += rx.matchedLength();
  }
  if (ret >= 0)
    rx.indexIn(str, ret);
  return ret;
}

QString
QtExt::increaseString(const QString &input)
{
  QRegExp rx("\\d+");
  int pos = matchLastString(input, rx);
  if (pos < 0)
    return input;

  QString cap = rx.cap();
  bool ok;
  uint n = cap.toUInt(&ok);
  if (!ok)
    return input;
  bool padding = cap.startsWith('0') &&
                 cap.length() != QString::number(n).length();
  n++;
  QString number = QString::number(n);
  if (padding) {
    int count = cap.length() - number.length();
    while (count--)
      number.prepend('0');
  }

  return input.left(pos) +
         number +
         input.mid(pos + rx.matchedLength());
}

QString
QtExt::decreaseString(const QString &input)
{
  QRegExp rx("\\d+");
  int pos = matchLastString(input, rx);
  if (pos < 0)
    return input;

  QString cap = rx.cap();
  bool ok;
  uint n = cap.toUInt(&ok);
  if (!ok || !n)
    return input;
  bool padding = cap.startsWith('0') &&
                 cap.length() != QString::number(n).length();
  n--;
  QString number = QString::number(n);
  if (padding) {
    int count = cap.length() - number.length();
    while (count--)
      number.prepend('0');
  }

  return input.left(pos) +
         number +
         input.mid(pos + rx.matchedLength());
}

// EOF
