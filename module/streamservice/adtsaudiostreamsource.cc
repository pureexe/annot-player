// adtsaudiostreamsource.cc
// 2/13/2011
// See: liveMedia/ADTSAudioStreamSource.cc

#include "adtsaudiostreamsource.h"
#include "module/stream/inputstream.h"
#include "module/mediacodec/mediatoc.h"
#include <GroupsockHelper.hh>
#include <QtCore>

#define DEBUG "adtsaudiostreamsource"
#include "module/debug/debug.h"

namespace { // anonymous
  unsigned const samplingFrequencyTable[16] = {
    96000, 88200, 64000, 48000,
    44100, 32000, 24000, 22050,
    16000, 12000, 11025, 8000,
    7350, 0, 0, 0
  };
} // anonymous namespace

ADTSAudioStreamSource*
ADTSAudioStreamSource::createNew(UsageEnvironment &env, InputStream *in,
                                 MediaToc *toc, float duration)
{
  Q_ASSERT(in);
  in->reset();
  // Now, having opened the input file, read the fixed header of the first frame,
  // to get the audio stream's parameters:
  unsigned char fixedHeader[4]; // it's actually 3.5 bytes long
  if (in->read((char*)fixedHeader, sizeof(fixedHeader))
      != sizeof(fixedHeader))
    return 0;

  // Check the 'syncword':
  if (!(fixedHeader[0] == 0xFF && (fixedHeader[1]&0xF0) == 0xF0)) {
    env.setResultMsg("Bad 'syncword' at start of ADTS file");
    return 0;
  }

  // Get and check the 'profile':
  u_int8_t profile = (fixedHeader[2]&0xC0)>>6; // 2 bits
  if (profile == 3) {
    env.setResultMsg("Bad (reserved) 'profile': 3 in first frame of ADTS file");
    return 0;
  }

  // Get and check the 'sampling_frequency_index':
  u_int8_t sampling_frequency_index = (fixedHeader[2]&0x3C)>>2; // 4 bits
  if (samplingFrequencyTable[sampling_frequency_index] == 0) {
    env.setResultMsg("Bad 'sampling_frequency_index' in first frame of ADTS file");
    return 0;
  }

  // Get and check the 'channel_configuration':
  u_int8_t channel_configuration
    = ((fixedHeader[2]&0x01)<<2)|((fixedHeader[3]&0xC0)>>6); // 3 bits

  // If we get here, the frame header was OK.
  // Reset the fid to the beginning of the file:
  in->reset();

//#ifdef DEBUG
//  fprintf(stderr, "Read first frame: profile %d, "
//      "sampling_frequency_index %d => samplingFrequency %d, "
//      "channel_configuration %d\n",
//      profile,
//      sampling_frequency_index, samplingFrequencyTable[sampling_frequency_index],
//      channel_configuration);
//#endif // DEBUG
  return new Self(env, in, profile, sampling_frequency_index, channel_configuration,
                  toc, duration);
}

ADTSAudioStreamSource
::ADTSAudioStreamSource(UsageEnvironment &env, InputStream *in, u_int8_t profile,
              u_int8_t samplingFrequencyIndex, u_int8_t channelConfiguration,
              MediaToc *toc, float duration)
  : Base(env, in, toc, duration)
{
  fSamplingFrequency = samplingFrequencyTable[samplingFrequencyIndex];
  fNumChannels = channelConfiguration == 0 ? 2 : channelConfiguration;
  fuSecsPerFrame
    = (1024/*samples-per-frame*/*1000000) / fSamplingFrequency/*samples-per-second*/;

  // Construct the 'AudioSpecificConfig', and from it, the corresponding ASCII string:
  unsigned char audioSpecificConfig[2];
  u_int8_t const audioObjectType = profile + 1;
  audioSpecificConfig[0] = (audioObjectType<<3) | (samplingFrequencyIndex>>1);
  audioSpecificConfig[1] = (samplingFrequencyIndex<<7) | (channelConfiguration<<3);
  sprintf(fConfigStr, "%02X%02x", audioSpecificConfig[0], audioSpecificConfig[1]);

  DOUT("feq =" << fSamplingFrequency << ", channels =" << fNumChannels << ", audioType =" << audioObjectType
       << ", config =" << (quint8)audioSpecificConfig[0]) << (quint8)audioSpecificConfig[1];
}

// Note: We should change the following to use asynchronous file reading, #####
// as we now do with ByteStreamFileSource. #####
void
ADTSAudioStreamSource::doGetNextFrame()
{
  // Begin by reading the 7-byte fixed_variable headers:
  unsigned char headers[7];
  if (in()->read((char*)headers, sizeof(headers)) != sizeof(headers)) {
    // The input source has ended:
    handleClosure(this);
    return;
  }

  // Extract important fields from the headers:
  Boolean protection_absent = headers[1]&0x01;
  u_int16_t frame_length
    = ((headers[3]&0x03)<<11) | (headers[4]<<3) | ((headers[5]&0xE0)>>5);
//#ifdef DEBUG
//  u_int16_t syncword = (headers[0]<<4) | (headers[1]>>4);
//  fprintf(stderr, "Read frame: syncword 0x%x, protection_absent %d, frame_length %d\n", syncword, protection_absent, frame_length);
//  if (syncword != 0xFFF) fprintf(stderr, "WARNING: Bad syncword!\n");
//#endif // DEBUG
  unsigned numBytesToRead
    = frame_length > sizeof headers ? frame_length - sizeof headers : 0;

  // If there's a 'crc_check' field, skip it:
  if (!protection_absent) {
    //SeekFile64(fFid, 2, SEEK_CUR);
    int count = in()->skip(2);
    numBytesToRead = numBytesToRead > (uint)count ? numBytesToRead - (uint)count : 0;
  }

  // Next, read the raw frame data into the buffer provided:
  if (numBytesToRead > fMaxSize) {
    fNumTruncatedBytes = numBytesToRead - fMaxSize;
    numBytesToRead = fMaxSize;
  }
  int numBytesRead = in()->read((char*)fTo, numBytesToRead);
  if (numBytesRead < 0) numBytesRead = 0;
  fFrameSize = numBytesRead;
  fNumTruncatedBytes += numBytesToRead - numBytesRead;

  // Set the 'presentation time':
  if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
    // This is the first frame, so use the current time:
    gettimeofday(&fPresentationTime, NULL);
  } else {
    // Increment by the play time of the previous frame:
    unsigned uSeconds = fPresentationTime.tv_usec + fuSecsPerFrame;
    fPresentationTime.tv_sec += uSeconds/1000000;
    fPresentationTime.tv_usec = uSeconds%1000000;
  }

  fDurationInMicroseconds = fuSecsPerFrame;

  // Switch to another task, and inform the reader that he has data:
  nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
                (TaskFunc*)FramedSource::afterGetting, this);
}

// EOF
