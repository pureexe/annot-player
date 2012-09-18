#ifndef STREAMPIPE_H
#define STREAMPIPE_H

// streampipe.h
// 2/22/2012

#include "lib/stream/inputstream.h"
#include "lib/stream/outputstream.h"
#include "qtx/qxstoppable.h"

class StreamPipe : public QxStoppableTask
{
  InputStream *in_;
  OutputStream *out_;

public:
  explicit StreamPipe(InputStream *in = nullptr, OutputStream *out = nullptr)
    : in_(in), out_(out) { }

public:
  void setInputStream(InputStream *in) { in_ = in; }
  void setOutputStream(OutputStream *out) { out_ = out; }

protected:
  InputStream *inputStream() const { return in_; }
  OutputStream *outputStream() const { return out_; }

};

class StreamListPipe : public QxStoppableTask
{
public:
  virtual void setOutputStream(OutputStream *out) = 0;
  virtual void setInputStreams(const InputStreamList &ins) = 0;

protected:
  virtual OutputStream *outputStream() const = 0;
  virtual InputStreamList inputStreams() const = 0;
};

#endif // STREAMPIPE_H
