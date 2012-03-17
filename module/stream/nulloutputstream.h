#ifndef NULLOUTPUTSTREAM_H
#define NULLOUTPUTSTREAM_H

// nulloutputstream.h
// 2/13/2012

#include "outputstream.h"
#include <QObject>

class NullOutputStream : public QObject, public OutputStream
{
  Q_OBJECT
  typedef NullOutputStream Self;
  typedef QObject Base;

public:
  explicit NullOutputStream(QObject *parent = 0)
    : Base(parent) { }

  virtual qint64 write(const char *data, qint64 maxSize) ///< \override
  { Q_UNUSED(data); return maxSize; }
};

#endif // FILEOUTPUTSTREAM_H
