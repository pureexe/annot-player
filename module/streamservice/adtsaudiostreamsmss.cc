// adtsaudiostreamsmss.cc
// 2/11/2012

#include "adtsaudiostreamsmss.h"
#include "adtsaudiostreamsource.h"
#include "live555_config.h"
#include <liveMedia.hh>
#include <GroupsockHelper.hh>

#define UNUSED(_var)    (void)(_var)
#define ASSERT(_var)

//#define DEBUG "adtsaudiostreamsmss"
#include "module/debug/debug.h"

// See: http://www.live555.com/liveMedia/faq.html
// See: vlc/modules/demux/live555.cpp
enum { AudioStreamBufferMaxSize = 1 * 1000 * 1000 };

// - Creation -

FramedSource*
ADTSAudioStreamSMSS::
createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate)
{
  UNUSED(clientSessionId);
  estBitrate = 96; // kbps, estimate
  return ADTSAudioStreamSource::createNew(envir(), in(), toc(), duration());
}

RTPSink*
ADTSAudioStreamSMSS::
createNewRTPSink(Groupsock *rtpGroupsock,
                 unsigned char rtpPayloadTypeIfDynamic,
                 FramedSource *inputSource)
{
  //ASSERT(rtpGroupsock);
  //::increaseSendBufferTo(envir(), rtpGroupsock->socketNum(), AudioStreamBufferMaxSize);
  //::increaseReceiveBufferTo(envir(), rtpGroupsock->socketNum(), AudioStreamBufferMaxSize);

  ADTSAudioStreamSource *adtsSource = dynamic_cast<ADTSAudioStreamSource*>(inputSource);
  ASSERT(adtsSource);
  return MPEG4GenericRTPSink::createNew(envir(), rtpGroupsock,
                    rtpPayloadTypeIfDynamic,
                    adtsSource->samplingFrequency(),
                    "audio", "AAC-hbr", adtsSource->configStr(),
                    adtsSource->numChannels());
}

// EOF
