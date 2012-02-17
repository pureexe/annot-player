// streamsmss.cc
// 2/13/2012

#include "streamsmss.h"
#include "framedstreamsource.h"
#include <liveMedia.hh>

#define UNUSED(_var)    (void)(_var)
#define ASSERT(_var)

// - Seek -

void
StreamSMSS::
seekStreamSource(FramedSource *inputSource, double &seekNPT,
                 double streamDuration, u_int64_t &numBytes)
{
  UNUSED(numBytes);
  UNUSED(streamDuration); // unimplemented
  FramedStreamSource *src = dynamic_cast<FramedStreamSource*>(inputSource);
  if (src)
    seekNPT = src->seek(seekNPT);
}

// EOF
