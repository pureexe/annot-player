// mp4codec.cc
// 2/14/2012

#include "mp4codec.h"
#include "aaccodec.h"
#include "module/datastream/inputstream.h"
#include "module/qtext/bitwise.h"
#include "module/mp4box/mp4box.h"
//#include <mp4v2/mp4v2.h>
#include <QtCore>

#define DEBUG "mp4codec"
#include "module/debug/debug.h"

// - Helper -

// See: http://www.jqueryphp.com/how-to-get-flv-file-duration/2009/08/
bool
MP4Codec::isMP4Stream(InputStream *input)
{
  Q_ASSERT(input);
  input->reset();
  if (input->size() < 3)
    return false;
  input->reset();
  char data[3];
  int count = input->read(data, sizeof(data));
  return count == 3 &&
      data[0] == 'M' &&
      data[1] == 'P' &&
      data[2] == '4';
}

// - Mux -

bool
MP4Codec::muxStream(const QString &path, qint64 duration, InputStream *vin, InputStream *ain)
{
  // TODO: CHECKPOINT
  // Interleave H.264 with AAC
  Q_UNUSED(path);
  Q_UNUSED(duration);
  Q_UNUSED(vin);
  Q_UNUSED(ain);
  Q_ASSERT(0);
  return false;

  /*
  DOUT("enter: duration =" << duration << ", path =" << path);

  MP4FileHandle mp4 = ::MP4Create(path.toLocal8Bit());
  if (mp4 == MP4_INVALID_FILE_HANDLE) {
    DOUT("exit: failed to create mp4 file");
    return false;
  }

  AACInfo aacInfo = AACCodec::parseAACInfo(ain);
  if (aacInfo.isBad()) {
    DOUT("failed to parse AAC info");
    return false;
  }

  bool ret = true;

  vin->reset();
  ain->reset();

  { // Video track
    ::MP4SetTimeScale(mp4, 60);
    ::MP4SetVideoProfileLevel(mp4, 0x7f);

    //MP4TrackId h264 = ::MP4AddH264VideoTrack(mp4, opt.videoTimeScale, opt.duration,

    // See: http://markmail.org/download.xqy?id=eg24hizyiez6jk4d&number=1
    //MP4TrackId h264 = ::MP4AddH264VideoTrack(mp4, opt.videoTimeScale, opt.duration,
    //  uint16_t      width,
    //  uint16_t      height,
    //  uint8_t       AVCProfileIndication,
    //  uint8_t       profile_compat,
    //  uint8_t       AVCLevelIndication,
    //  uint8_t       sampleLenFieldSizeMinusOne );
    //bool ret = ::MP4WriteSample(mp4, vt, (quint8*)vin->readAll().data(), vin->size());

    MP4TrackId tid = ::MP4AddH264VideoTrack(
           mp4,
           60,
           1, //MP4_INVALID_DURATION
           1280, 720,
           0x4d,
           0x00,
           0x28,
           3 );
       if( tid == MP4_INVALID_TRACK_ID ) {
           DOUT("MP4AddH264VideoTrack error");
           return false;
       }

      ret = ret && ::MP4WriteSample(mp4, tid, (quint8*)vin->readAll().data(), vin->size());
  }

  { // Audio track
    ::MP4SetTimeScale(mp4, aacInfo.frequency);
    ::MP4SetAudioProfileLevel(mp4, 0x0f);
    MP4TrackId aac = ::MP4AddAudioTrack(mp4, aacInfo.frequency, duration);
    if (aac == MP4_INVALID_TRACK_ID)
      DOUT("failed to add audio track");
    else {
      // See: foobar2000/foo_faac.c
      // See: faac/frame.c
      // See: http://pastie.org/pastes/357314/text

      enum { aac_config_size = 2 }; // sizeof(aacInfo.config / *aacInfo.config[0]);
      ::MP4SetTrackESConfiguration(mp4, aac, aacInfo.config, aac_config_size);
      ret = ret && ::MP4WriteSample(mp4, aac, (quint8*)ain->readAll().data(), ain->size());
    }
  }

  ::MP4Close(mp4);
  DOUT("exit: ret =" << ret);
  return ret;
  */
}

// EOF
