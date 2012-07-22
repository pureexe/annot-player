#ifndef DATAINPUTSTREAM_H
#define DATAINPUTSTREAM_H

// datainputstream.h
// 3/14/2012

#include "module/stream/inputstream.h"
#include <QtCore/QByteArray>
#include <QtCore/QObject>

class DataInputStream : public QObject, public InputStream
{
  Q_OBJECT
  Q_DISABLE_COPY(DataInputStream)
  typedef DataInputStream Self;
  typedef QObject Base;

  const QByteArray &data_;
  qint64 pos_;

public:
  explicit DataInputStream(const QByteArray &data, QObject *parent = nullptr)
    : Base(parent), data_(data), pos_(0) { }

  const QByteArray &data() const { return data_; }

public:
  qint64 size() const override { return data_.size(); }
  qint64 pos() const override { return pos_; }

  bool reset()  override { pos_ = 0; return true; }

  bool seek(qint64 pos) override
  { if (pos_ >= data_.size()) return false; pos_ = pos; return true; }

  qint64 skip(qint64 count) override
  { qint64 diff = qMin(count, data_.size() - pos_); pos_ += diff; return diff; }

  qint64 read(char *data, qint64 maxSize) override;

  QByteArray readAll() override
  { return pos_ ? data_.mid(pos_) : data_; }

  bool writeFile(const QString &path) override;
};

#endif // DATAINPUTSTREAM_H
