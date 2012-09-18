// qxstring.cc
// 3/11/2012

#include "qtx/qxstring.h"

QTX_BEGIN_NAMESPACE

int
qxMatchLastString(const QString &str, QRegExp &rx)
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
qxIncreaseString(const QString &input)
{
  QRegExp rx("\\d+");
  int pos = qxMatchLastString(input, rx);
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
qxDecreaseString(const QString &input)
{
  QRegExp rx("\\d+");
  int pos = qxMatchLastString(input, rx);
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

QTX_END_NAMESPACE
