#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

// inputstream.h
// 2/9/2012

#include "module/qtext/bitwise.h"
#include <QtCore/QList>
#include <QtCore/QString>

class InputStream
{
public:
  virtual ~InputStream() { }

  bool isEmpty() const { return !size(); }

  bool atEnd() const
  { qint64 s = size(); return s && pos() >= s; }

  virtual void setBufferSize(qint64 size) { Q_UNUSED(size); }

  virtual qint64 size() const { return 0; }
  virtual qint64 pos() const { return 0; }

  virtual qint64 read(char *data, qint64 maxSize) = 0;
  virtual qint64 tryRead(char *data, qint64 maxSize) { Q_UNUSED(data); Q_UNUSED(maxSize); return 0; }

  qint64 read(QByteArray &data)
  { return read(data.data(), data.size()); }

  virtual bool reset() = 0;

  virtual bool seek(qint64 pos) { Q_UNUSED(pos); return false; }
  virtual qint64 skip(qint64 count) { Q_UNUSED(count); return 0; }

  virtual QByteArray readAll();

  ///  Warning: default implementation will change the pos().
  virtual bool writeFile(const QString &path);

  // - Bitwise -
public:

  qint64 read(quint8 *data, qint64 count)
  { return read(reinterpret_cast<char *>(data), count); }

  quint8 readUInt8(bool *ok = nullptr)
  {
    quint8 x = 0;
    if (ok)
      *ok = read(&x, 1) == 1;
    else
      read(&x, 1);
    return x;
  }

  quint16 readUInt16(bool *ok = nullptr)
  {
    quint8 x[2] = { };
    if (ok)
      *ok = read(x, 2) == 2;
    else
      read(x, 2);
    return Bitwise::BigEndian::toUInt16(x);
  }

  quint32 readUInt24(bool *ok = nullptr)
  {
    quint8 x[4] = { };
    if (ok)
      *ok = read(x + 1, 3) == 3;
    else
      read(x + 1, 3);
    return Bitwise::BigEndian::toUInt32(x);
  }

  quint32
  readUInt32(bool *ok = nullptr)
  {
    quint8 x[4] = { };
    if (ok)
      *ok = read(x, 4) == 4;
    else
      read(x, 4);
    return Bitwise::BigEndian::toUInt32(x);
  }

  quint64 readUInt64(bool *ok = nullptr)
  {
    quint8 x[8] = { };
    if (ok)
      *ok = read(x, 8) == 8;
    else
      read(x, 8);
    return Bitwise::BigEndian::toUInt64(x);
  }

  float readFloat(bool *ok = nullptr)
  {
    enum { size = sizeof(float) }; // 4
    quint8 x[size] = { };
    if (ok)
      *ok = read(x, size) == size;
    else
      read(x, size);
    return Bitwise::BigEndian::toFloat(x);
  }

  double readDouble(bool *ok = nullptr)
  {
    enum { size = sizeof(double) }; // 8
    quint8 x[size] = { };
    if (ok)
      *ok = read(x, size) == size;
    else
      read(x, size);
    return Bitwise::BigEndian::toDouble(x);
  }
};

class InputStreamList : public QList<InputStream *>
{
  typedef InputStreamList Self;
  typedef QList<InputStream *> Base;
public:
  InputStreamList() { }
  InputStreamList(const Self &that) : Base(that) { }

  void reset() const
  { foreach (value_type v, *this) v->reset(); }
};

#endif // INPUTSTREAM_H
