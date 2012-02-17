// framedstreamsource.cc
// 2/12/2012
// See: ByteStreamFileSource.cpp

#include "framedstreamsource.h"
#include "module/datastream/inputstream.h"
#include "module/mediacodec/mediatoc.h"
#include "live555_config.h"
#include <liveMedia.hh>
#include <ctime>

#ifdef Q_WS_WIN
  #include "module/qtwin/qtwin.h"
  using QtWin::gettimeofday;
#endif // Q_WS_WIN

// - Do -

void
FramedStreamSource::doStopGettingFrames() {  }

void
FramedStreamSource::doGetNextFrame()
{
  //if (in_->isEmpty() || in_->atEnd()) {
  //  handleClosure(this);
  //  return;
  //}

  fFrameSize = in_->read((char*)fTo, fMaxSize);
  if (!fFrameSize) {
    handleClosure(this);
    return;
  }

  gettimeofday(&fPresentationTime, 0);

  // Inform the reader that he has data:
  // To avoid possible infinite recursion, we need to return to the event loop to do this:
  //FramedSource::afterGetting(this);
  nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
                (TaskFunc*)FramedSource::afterGetting, this);
}

// - Seek -
#include <QDebug>
double
FramedStreamSource::seek(double seconds)
{
  if (duration_ <= 0 || !toc_)
    return seconds;
  qint64 msecs = seconds * 1000;
  qint64 offset = toc_->offsetByTime(msecs);
  if (offset >= 0)
    in()->seek(offset);
  //seconds = duration_ * (double)offset / size;
  return msecs / 1000.0;
}

// EOF

/*
void ByteStreamRemoteSource::doStopGettingFrames() {
#ifndef READ_FROM_FILES_SYNCHRONOUSLY
  envir().taskScheduler().turnOffBackgroundReadHandling(fileno(fFid));
  fHaveStartedReading = False;
#endif
}

*/
