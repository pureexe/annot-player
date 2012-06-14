#ifndef ADTSAUDIOSTREAMSOURCE_H
#define ADTSAUDIOSTREAMSOURCE_H

// adtsaudiostreamsource.h
// See: liveMedia/ADTSAudioFileSource.hh
// 2/13/2012

#include "framedstreamsource.h"

class MediaToc;

class ADTSAudioStreamSource: public FramedStreamSource
{
  typedef ADTSAudioStreamSource Self;
  typedef FramedStreamSource Base;

  MediaToc *toc_;
  float duration_;

public:
  static ADTSAudioStreamSource* createNew(UsageEnvironment &env, InputStream *in, MediaToc *toc = 0, float duration = 0);

  unsigned samplingFrequency() const { return fSamplingFrequency; }
  unsigned numChannels() const { return fNumChannels; }
  char const* configStr() const { return fConfigStr; }
      // returns the 'AudioSpecificConfig' for this stream (in ASCII form)

private:
  ADTSAudioStreamSource(UsageEnvironment& env,
                        InputStream *in, u_int8_t profile,
                        u_int8_t samplingFrequencyIndex,
                        u_int8_t channelConfiguration,
                        MediaToc *toc,
                        float duration);

private:
  // redefined virtual functions:
  virtual void doGetNextFrame(); ///< \reimp

private:
  unsigned fSamplingFrequency;
  unsigned fNumChannels;
  unsigned fuSecsPerFrame;
  char fConfigStr[5];
};

#endif // ADTSAUDIOSTREAMSOURCE_H
