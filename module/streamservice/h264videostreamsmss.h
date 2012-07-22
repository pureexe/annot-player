#ifndef H264VIDEOSTREAMSMSS_H
#define H264VIDEOSTREAMSMSS_H

// h264videostreamsmss.h
// 2/11/2012

#include "streamsmss.h"

class H264VideoStreamSMSS : public StreamSMSS
{
  typedef H264VideoStreamSMSS Self;
  typedef StreamSMSS Base;

  // - Construction -
public:
  static Self *createNew(UsageEnvironment &env, InputStream *in, MediaToc *toc = nullptr, float duration = 0)
  { return new Self(env, in, toc, duration); }

protected:
  H264VideoStreamSMSS(UsageEnvironment &env, InputStream *in, MediaToc *toc, float duration)
    : Base(env, in, toc, duration) { }

protected:
  FramedSource *createNewStreamSource(unsigned clientSessionId,
                                      unsigned &estBitrate) override;
  RTPSink *createNewRTPSink(Groupsock *rtpGroupsock,
                            unsigned char rtpPayloadTypeIfDynamic,
                            FramedSource *inputSource) override;
  void seekStreamSource(FramedSource* inputSource,
                        double &seekNPT,
                        double streamDuration,
                        u_int64_t &numBytes) override;
};

#endif // H264VIDEOSTREAMSMSS_H
