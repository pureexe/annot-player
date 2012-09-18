#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

// outputstream.h
// 2/9/2012

#include "qtx/qxbitwise.h"
#include <QtCore/QByteArray>
#include <QtCore/QList>

class OutputStream
{
public:
  virtual ~OutputStream() { }

  virtual void setBufferSize(qint64 size) { Q_UNUSED(size) }

  virtual qint64 availableSize() const { return 0; }

  virtual qint64 write(const char *data, qint64 maxSize) = 0;
  qint64 write(const QByteArray &data) { return write(data.constData(), data.size()); }
  virtual void flush() { }
  virtual void finish() { flush(); }

  // - QxBitwise -
public:
  qint64 write(const quint8 *data, qint64 count)
  { return write(reinterpret_cast<const char *>(data), count); }

  int writeUInt8(quint8 value)
  { return write(&value, 1); }

  int writeUInt16(quint16 value)
  {
    quint8 x[2];
    QxBitwise::BigEndian::getBytes(x, value);
    return write(x, 2);
  }

  int writeUInt24(quint32 value)
  {
    quint8 x[4];
    QxBitwise::BigEndian::getBytes(x, value);
    return write(x + 1, 3);
  }

  int writeUInt32(quint32 value)
  {
    quint8 x[4];
    QxBitwise::BigEndian::getBytes(x, value);
    return write(x, 4);
  }

  int writeUInt64(quint64 value)
  {
    quint8 x[8];
    QxBitwise::BigEndian::getBytes(x, value);
    return write(x, 8);
  }

  int writeFloat(float value)
  {
    enum { size = sizeof(float) }; // 4
    quint8 x[size];
    QxBitwise::BigEndian::getBytes(x, value);
    return write(x, size);
  }

  int writeDouble(double value)
  {
    enum { size = sizeof(double) }; // 8
    quint8 x[size];
    QxBitwise::BigEndian::getBytes(x, value);
    return write(x, size);
  }
};

typedef QList<OutputStream*> OutputStreamList;

#endif // OUTPUTSTREAM_H
