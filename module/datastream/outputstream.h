#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

// outputstream.h
// 2/9/2012

#include <QtGlobal>
#include <QList>

class OutputStream
{
public:
  virtual ~OutputStream() { }

  virtual void setBufferSize(qint64 size) { Q_UNUSED(size); }

  virtual qint64 realSize() const { return 0; }

  virtual qint64 write(const char *data, qint64 maxSize) = 0;
  virtual void flush() { }
  virtual void finish() { flush(); }
};

typedef QList<OutputStream*> OutputStreamList;

#endif // OUTPUTSTREAM_H
