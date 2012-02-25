#ifndef STREAMPIPE_H
#define STREAMPIPE_H

// streampipe.h
// 2/22/2012

#include "inputstream.h"
#include "outputstream.h"
#include <QRunnable>

class StreamPipe : public QRunnable
{
  InputStream *in_;
  OutputStream *out_;

public:
  StreamPipe(InputStream *in, OutputStream *out)
    : in_(in), out_(out) { Q_ASSERT(in_ && out_); }

  virtual ~StreamPipe() { }

protected:
  InputStream *inputStream() const { return in_; }
  OutputStream *outputStream() const { return out_; }
};

#endif // STREAMPIPE_H
