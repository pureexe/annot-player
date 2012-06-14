#ifndef FIFOSTREAM_H
#define FIFOSTREAM_H

// fifostream.h
// 2/9/2012

#include "module/stream/inputstream.h"
#include "module/stream/outputstream.h"

class FifoStream : public InputStream, public OutputStream
{
  qint64 size_;
public:
  explicit FifoStream(qint64 size = 0) : size_(size) { }

  virtual qint64 size() const { return size_; } ///< \reimp

  void setSize(qint64 size) { size_ = size; }
};

#endif // FIFOSTREAM_H
