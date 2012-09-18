// securebufferedfifostream.cc
// 3/14/2012

#include "lib/stream/securebufferedfifostream.h"
#include <QtCore/QMutexLocker>

//#define DEBUG "securebufferedfifostream"
#include "qtx/qxdebug.h"

// - Constrution -

SecureBufferedFifoStream::SecureBufferedFifoStream(QObject *parent)
  : Base(parent)
{
  connect(this, SIGNAL(readyRead()), SLOT(wakeAllReadyRead()));
  connect(this, SIGNAL(finished()), SLOT(wakeAllFinished()));
}

// - Wait -
void
SecureBufferedFifoStream::waitForReadyRead()
{
  DOUT("enter");
  QMutex m;
  QMutexLocker ml(&m);
  readyReadCond_.wait(&m);
  DOUT("exit");
}

void
SecureBufferedFifoStream::waitForFinished()
{
  DOUT("enter");
  if (isFinished()) {
    DOUT("exit: already finished");
    return;
  }
  QMutex m;
  QMutexLocker ml(&m);
  finishedCond_.wait(&m);
  DOUT("exit");
}

// EOF
