#ifndef DATAOUTPUTSTREAM_H
#define DATAOUTPUTSTREAM_H

// dataoutputstream.h
// 3/14/2012

#include "module/stream/outputstream.h"
#include <QtCore/QByteArray>
#include <QtCore/QObject>

class DataOutputStream : public QObject, public OutputStream
{
  Q_OBJECT
  Q_DISABLE_COPY(DataOutputStream)
  typedef DataOutputStream Self;
  typedef QObject Base;

  QByteArray data_;

public:
  explicit DataOutputStream(QObject *parent = 0)
    : Base(parent) { }

  const QByteArray &data() const { return data_; }
  QByteArray &data() { return data_; }

public:
  virtual qint64 availableSize() const { return data_.size(); } ///< \override

  virtual qint64 write(const char *data, qint64 maxSize) ///< \override
  { data_.append(data, maxSize); return maxSize; }
};

#endif // DATAOUTPUTSTREAM_H
