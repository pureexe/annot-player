// h264writer.cc
// 2/12/2012

#include "mediawriter.h"
#include "mediatoc.h"
#include "module/qtext/bitwise.h"

#define DEBUG "h264writer"
#include "module/debug/debug.h"

namespace Bitwise { using namespace BigEndian; }

void
RawH264Writer::writeFrame(const QByteArray &chunk, quint32 timestamp, bool writeHeader)
{
  static const quint8 StartCode[] = { 0, 0, 0, 1 };
  enum { StartCodeSize = sizeof(StartCode)/sizeof(*StartCode) };
  if (chunk.size() < 4)
    return;

  // See: decode_frame in ffmpeg/libavcodec/h264.c

  if (chunk[0] == 0) { // Headers
    if (chunk.size() < 10)
      return;

    int offset = 8;
    nalLengthSize_ = (chunk[offset++] & 0x03) + 1;
    if (!writeHeader)
      return;
    if (toc_)
      toc_->append(out_->availableSize(), timestamp);
    int spsCount = chunk[offset++] & 0x1F;
    int ppsCount = -1;

    while (offset <= chunk.size() - 2) {
      if ((spsCount == 0) && (ppsCount == -1)) {
        ppsCount = chunk[offset++];
        continue;
      }

      if (spsCount > 0) spsCount--;
      else if (ppsCount > 0) ppsCount--;
      else break;

      const quint8 *p = (const quint8 *)chunk.data();
      int len = Bitwise::toUInt16(p, offset);
        offset += 2;
      if (offset + len > chunk.size()) break;
      out_->write(StartCode, StartCodeSize);
      out_->write(chunk.data() + offset, len);
      offset += len;
    }
  } else { // Video data
    if (toc_)
      toc_->append(out_->availableSize(), timestamp);

    int offset = 4;

    if (nalLengthSize_ != 2)
      nalLengthSize_ = 4;

    const quint8 *p = (const quint8 *)chunk.constData();
    while (offset <= chunk.size() - nalLengthSize_) {
      int len = nalLengthSize_ == 2 ?
        (int)Bitwise::toUInt16(p, offset) :
        (int)Bitwise::toUInt32(p, offset);
      offset += nalLengthSize_;
      if (offset + len > chunk.size()) break;
      out_->write(StartCode, StartCodeSize);
      out_->write(chunk.data() + offset, len);
      offset += len;
    }
  }
}

// EOF
