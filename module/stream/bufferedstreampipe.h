#ifndef BUFFEREDSTREAMPIPE_H
#define BUFFEREDSTREAMPIPE_H

// bufferedstreampipe.h
// 2/22/2012

#include "module/stream/streampipe.h"
#include "module/qtext/stoppable.h"
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

  BufferedStreamPipe(InputStream *in, OutputStream *out, QObject *parent = 0)
    : Base(parent), StreamPipe(in, out), bufferSize_(DefaultBufferSize), state_(Stopped)
  { }

signals:
  void finished();
  void stopped();
  void error(QString message);

public:
  int bufferSize() const { return bufferSize_; }
  State state() const { return state_; }

  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Running; }
  bool isFinished() const { return state_ == Finished; }

public slots:
  void setBufferSize(int size) { bufferSize_ = size; }

  virtual void run(); ///< \reimp
  virtual void stop() { setState(Stopped); emit stopped(); } ///< \reimp

protected:
  void setState(State state) { state_ = state; }

private:
  int bufferSize_;
  State state_;
};

#endif // BUFFEREDSTREAMPIPE_H
