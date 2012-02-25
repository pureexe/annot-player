// bufferedstreampipe.cc
// 2/22/2012

#include "bufferedstreampipe.h"
#include <QtCore>

#define DEBUG "bufferedstreampipe"
#include "module/debug/debug.h"

// - Task -

void
BufferedStreamPipe::run()
{
  DOUT("enter: bufferSize =" << bufferSize_);
  Q_ASSERT(bufferSize_ > 0);
  setState(Running);

  char *buf = new char[bufferSize_];
  Q_ASSERT(buf);
  int count;
  while ((count = inputStream()->read(buf, bufferSize_)) == bufferSize_ &&
         !isStopped()) {
    //DOUT("count =" << count);
    if (count <= 0)
      break;
    else {
      count = outputStream()->write(buf, bufferSize_);
      if (count != bufferSize_) {
        setState(Error);
        emit error(tr("failed to write to outputstream"));
        delete[] buf;
        DOUT("exit: ERROR: failed to write to outputstream, count =" << count);
        return;
      }
    }
  }
  DOUT("count =" << count);

  if (count > 0 &&
      count != outputStream()->write(buf, count)) {
    setState(Error);
    emit error(tr("failed to write to outputstream"));
    delete[] buf;
    DOUT("exit: ERROR: failed to write to outputstream, count =" << count);
    return;
  }

  outputStream()->finish();
  delete[] buf;
  if (isRunning())
    setState(Finished);
  emit finished();
  DOUT("exit");
}


// EOF
