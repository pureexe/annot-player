#ifndef SECUREBUFFEREDFIFOSTREAM_H
#define SECUREBUFFEREDFIFOSTREAM_H

// securebufferedfifostream.h
// 3/14/2012

#include "bufferedfifostream.h"
#include <QtCore/QWaitCondition>

class SecureBufferedFifoStream : public BufferedFifoStream
{
  Q_OBJECT
  typedef SecureBufferedFifoStream Self;
  typedef BufferedFifoStream Base;

  QWaitCondition readyReadCond_, finishedCond_;

public:
  explicit SecureBufferedFifoStream(QObject *parent = 0);

public slots:
  virtual void waitForFinished(); ///< \override
  virtual void waitForReadyRead(); ///< \override

protected slots:
  void wakeAllReadyRead() { readyReadCond_.wakeAll(); }
  void wakeAllFinished() { finishedCond_.wakeAll(); }
};

#endif // SECUREBUFFEREDFIFOSTREAM_H
