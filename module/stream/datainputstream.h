#ifndef DATAINPUTSTREAM_H
#define DATAINPUTSTREAM_H

// datainputstream.h
// 3/14/2012

#include "inputstream.h"
#include <QtCore/QObject>
#include <QtCore/QByteArray>

class DataInputStream : public QObject, public InputStream
{
  Q_OBJECT
  typedef DataInputStream Self;
  typedef QObject Base;

  const QByteArray &data_;
  qint64 pos_;

public:
  explicit DataInputStream(const QByteArray &data, QObject *parent = 0)
    : Base(parent), data_(data), pos_(0) { }

  const QByteArray &data() const { return data_; }

public:
  virtual qint64 size() const { return data_.size(); } ///< \override
  virtual qint64 pos() const { return pos_; } ///< \override

  virtual bool reset()  { pos_ = 0; return true; } ///< \override

  virtual bool seek(qint64 pos) ///< \override
  { if (pos_ >= data_.size()) return false; pos_ = pos; return true; }

  virtual qint64 skip(qint64 count) ///< \override
  { qint64 diff = qMin(count, data_.size() - pos_); pos_ += diff; return diff; }

  virtual qint64 read(char *data, qint64 maxSize); ///< \override

  virtual QByteArray readAll()  ///< \override
  { return pos_ ? data_.mid(pos_) : data_; }

  virtual bool writeFile(const QString &path); ///< \override
};

#endif // DATAINPUTSTREAM_H
