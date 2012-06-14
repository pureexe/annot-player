#ifndef FRAMEDSTREAMSOURCE_H
#define FRAMEDSTREAMSOURCE_H

// framedstreamsource.h
// 2/12/2012
// See: liveMedia/ByteStreamFileSource.hh

#include "live555_config.h"
#include <FramedSource.hh>

class InputStream;
class MediaToc;

class FramedStreamSource: public FramedSource
{
  typedef FramedStreamSource Self;
  typedef FramedSource Base;

  InputStream *in_;
  MediaToc *toc_;
  float duration_;

  // - Construction -
public:
  static Self *createNew(UsageEnvironment &env, InputStream *in,
                         MediaToc *toc = 0, float duration = 0)
  { return new Self(env, in, toc, duration); }

protected:
  FramedStreamSource(UsageEnvironment &env, InputStream *in,
                     MediaToc *toc, float duration)
    : Base(env), in_(in), toc_(toc), duration_(duration) { }


public:
  double seek(double seconds);

protected:
  InputStream *in() const { return in_; }

private:
  virtual void doGetNextFrame(); ///< \reimp
  virtual void doStopGettingFrames(); ///< \reimp
};

#endif // FRAMEDSTREAMSOURCE_H
