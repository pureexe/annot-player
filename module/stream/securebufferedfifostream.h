#ifndef SECUREBUFFEREDFIFOSTREAM_H
#define SECUREBUFFEREDFIFOSTREAM_H

// securebufferedfifostream.h
// 3/14/2012

#include "module/stream/bufferedfifostream.h"
#include <QtCore/QWaitCondition>

class SecureBufferedFifoStream : public BufferedFifoStream
{
  Q_OBJECT
  Q_DISABLE_COPY(SecureBufferedFifoStream)
  typedef SecureBufferedFifoStream Self;
  typedef BufferedFifoStream Base;

  QWaitCondition readyReadCond_, finishedCond_;

public:
  explicit SecureBufferedFifoStream(QObject *parent = nullptr);

public slots:
  void waitForFinished() override;
  void waitForReadyRead() override;

protected slots:
  void wakeAllReadyRead() { readyReadCond_.wakeAll(); }
  void wakeAllFinished() { finishedCond_.wakeAll(); }
};

#endif // SECUREBUFFEREDFIFOSTREAM_H
