#ifndef BUFFEREDSTREAMPIPE_H
#define BUFFEREDSTREAMPIPE_H

// bufferedstreampipe.h
// 2/22/2012

#include "lib/stream/streampipe.h"
#include "qtx/qxstoppable.h"
#include <QtCore/QObject>
#include <QtCore/QString>

class BufferedStreamPipe : public QObject, public StreamPipe
{
  Q_OBJECT
  Q_DISABLE_COPY(BufferedStreamPipe)
  typedef BufferedStreamPipe Self;
  typedef QObject Base;

public:
  enum State { Error = -1, Stopped = 0, Running, Finished };
  enum { DefaultBufferSize = 10240 };

  BufferedStreamPipe(InputStream *in, OutputStream *out, QObject *parent = nullptr)
    : Base(parent), StreamPipe(in, out), bufferSize_(DefaultBufferSize), state_(Stopped)
  { }

signals:
  void finished();
  void stopped();
  void errorMessage(QString message);

public:
  int bufferSize() const { return bufferSize_; }
  State state() const { return state_; }

  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Running; }
  bool isFinished() const { return state_ == Finished; }

public slots:
  void setBufferSize(int size) { bufferSize_ = size; }

  void run() override;
  void stop() override { setState(Stopped); emit stopped(); }

protected:
  void setState(State state) { state_ = state; }

private:
  int bufferSize_;
  State state_;
};

#endif // BUFFEREDSTREAMPIPE_H
