// h264videostreamsmss.cc
// 2/11/2012

#include "h264videostreamsmss.h"
#include "framedstreamsource.h"
#include <liveMedia.hh>
#include <GroupsockHelper.hh>

#include <QtCore>
#define DEBUG "h264videostreamsmss"
#include "module/debug/debug.h"

#define UNUSED(_var)    (void)(_var)
#define ASSERT(_var)

// See: http://www.live555.com/liveMedia/faq.html
// See: liveMedia/MultiFramedRTPSource.cpp
// See: vlc/modules/demux/live555.cpp
//enum { VideoStreamBufferMaxSize = 2 * 1000 * 1000 };

// - Creation -

FramedSource*
H264VideoStreamSMSS::
createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate)
{
  UNUSED(clientSessionId);
  estBitrate = 500; // kbps, estimate
  DOUT("est bitrate =" << estBitrate << "kbps");

  // Create the video source:
  //ByteStreamFileSource* fileSource = ByteStreamFileSource::createNew(envir(), "/Users/jichi/tmp/bilibili.flv");
  //if (fileSource == NULL) return NULL;

  return H264VideoStreamFramer::createNew(envir(),
    FramedStreamSource::createNew(envir(), in(), toc(), duration())
  );
}

RTPSink*
H264VideoStreamSMSS::
createNewRTPSink(Groupsock *rtpGroupsock,
                 unsigned char rtpPayloadTypeIfDynamic,
                 FramedSource *inputSource)
{
  //::increaseReceiveBufferTo(envir(), rtpGroupsock->socketNum(), VideoStreamBufferMaxSize);
  //::increaseReceiveBufferTo(envir(), rtpGroupsock->socketNum(), VideoStreamBufferMaxSize);
  UNUSED(inputSource);
  ASSERT(rtpGroupsock);
  return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}

// - Seek -

void
H264VideoStreamSMSS::
seekStreamSource(FramedSource *inputSource, double &seekNPT,
                 double streamDuration, u_int64_t &numBytes)
{
  H264VideoStreamFramer *src = dynamic_cast<H264VideoStreamFramer *>(inputSource);
  if (src) {
    Base::seekStreamSource(src->inputSource(), seekNPT, streamDuration, numBytes);
    src->flushInput();
  }
}

// EOF
