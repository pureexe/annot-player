#ifndef QGZIP_H
#define QGZIP_H

// qgzip.h
// 2/3/2012

#include <QByteArray>


QByteArray gUncompress(const QByteArray &data, int zWindowBits = 0);

inline QByteArray gHttpUncompress(const QByteArray &data)
{ return gUncompress(data, -15); }

#endif // QGZIP
