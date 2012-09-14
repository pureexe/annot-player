#ifndef TEXTCODEC_H
#define TEXTCODEC_H

// textcodec.h
// 6/16/2012

#include <QtCore/QString>

namespace TextCodec {

  // - Conversion -
  QString zhs2zht(const QString &utf8);
  QString zht2zhs(const QString &utf8);

  QString hira2kata(const QString &utf8);
  QString kata2hira(const QString &utf8);

} // namespace TextCodec

#endif // TEXTCODEC_H
