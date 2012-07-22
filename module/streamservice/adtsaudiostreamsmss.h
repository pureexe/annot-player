#ifndef ADTSAUDIOSTREAMSMSS_H
#define ADTSAUDIOSTREAMSMSS_H

// adtsaudiostreamsmss.h
// 2/11/2012

#include "streamsmss.h"

class ADTSAudioStreamSMSS : public StreamSMSS
{
  typedef ADTSAudioStreamSMSS Self;
  typedef StreamSMSS Base;

  // - Construction -
public:
  static Self *createNew(UsageEnvironment &env, InputStream *in, MediaToc*toc, float duration = 0)
  { return new Self(env, in, toc, duration); }

protected:
  ADTSAudioStreamSMSS(UsageEnvironment &env, InputStream *in, MediaToc *toc, float duration)
    : Base(env, in, toc, duration) { }

protected:
  FramedSource *createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate) override;
  RTPSink *createNewRTPSink(Groupsock *rtpGroupsock,
                            unsigned char rtpPayloadTypeIfDynamic,
                            FramedSource *inputSource) override;
};

#endif // ADTSAUDIOSTREAMSMSS_H
