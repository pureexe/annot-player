#ifndef MP4CODEC_H
#define MP4CODEC_H

// mp4codec.h
// 2/12/2012

#include "mediacodec_config.h"
#include <QString>
#include <utility>

class InputStream;

namespace Mp4Codec
{
  //bool muxStream(const QString &path, qint64 duration, InputStream *vin, InputStream *ain);

#ifdef WITH_MP4V2
  ///  Return (width, height) of the video track.
  std::pair<int, int> fileDimension(const QString &path);
  QString fileInfo(const QString &path);
#endif // WITH_MP4V2

  bool isMp4Stream(InputStream *in); // Only for verification, not fully implemented.
  bool isMp4File(const QString &fileName);
}

#endif // MP4CODEC_H
