// mp4codec.cc
// 2/14/2012

#include "mp4codec.h"
#include "module/stream/inputstream.h"
#include "module/stream/fileinputstream.h"
#ifdef WITH_MP4V2
#  include <mp4v2/mp4v2.h>
#endif // WITH_MP4V2
#include <QtCore>
#include <cstdlib>
#include <cstring>

#define DEBUG "mp4codec"
#include "module/debug/debug.h"

// See: http://www.jqueryphp.com/how-to-get-flv-file-duration/2009/08/
bool
Mp4Codec::isMp4Stream(InputStream *input)
{
  Q_ASSERT(input);
  input->reset();
  if (input->size() < 8)
    return false;
  input->reset();
  char data[8];
  enum { size = sizeof(data)/sizeof(*data) };
  int count = input->read(data, sizeof(data));
  // xxxx xxxx
  // 6674 7970 6973 6f6d: ftypisom
  // 0000 0200
  // ...................: isomiso2avc1mp41
  return count == size &&
      data[4] == 'f' && data[5] == 't' && data[6] == 'y' && data[7] == 'p';
}

bool
Mp4Codec::isMp4File(const QString &fileName)
{
  FileInputStream fin(fileName);
  return isMp4Stream(&fin);
}

#ifdef WITH_MP4V2
std::pair<int, int>
Mp4Codec::fileDimension(const QString &path)
{
  std::pair<int, int> ret;
  MP4FileHandle mp4 = ::MP4Read(path.toLocal8Bit());
  if (mp4 == MP4_INVALID_FILE_HANDLE)
    return ret;
  int trackNum = ::MP4GetNumberOfTracks(mp4);
  for (int index = 0; index < trackNum; index++) {
    MP4TrackId trackId = ::MP4FindTrackId(mp4, index);
    if (trackId == MP4_INVALID_TRACK_ID)
      continue;
    if (!::strcmp(::MP4GetTrackType(mp4, trackId), MP4_VIDEO_TRACK_TYPE)) {
      ret.first = ::MP4GetTrackVideoWidth(mp4, trackId);
      ret.second = ::MP4GetTrackVideoWidth(mp4, trackId);
      break;
    }
  }
  ::MP4Close(mp4);
  return ret;
}

QString
Mp4Codec::fileInfo(const QString &path)
{
  QString ret;
  char *info = ::MP4FileInfo(path.toLocal8Bit());
  if (info) {
    ret = QString::fromLocal8Bit(info);
    ::free(info);
  }
  return ret;
}

#endif // WITH_MP4V2

// EOF

// - Mux -

/*
bool
Mp4Codec::muxStream(const QString &path, qint64 duration, InputStream *vin, InputStream *ain)
{
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
}
*/
