// h264writer.cc
// 2/12/2012

#include "mediawriter.h"
#include "mediatoc.h"
#include "module/qtext/bitwise.h"
#include <QtCore>

#define DEBUG "h264writer"
#include "module/debug/debug.h"

namespace Bitwise { using namespace BigEndian; }

void
RawH264Writer::writeFrame(const QByteArray &chunk, quint32 timestamp, bool writeHeader)
{
  static const quint8 _startCode[4] = { 0, 0, 0, 1 };
  enum { _startCodeSize = sizeof(_startCode)/sizeof(*_startCode) };
  if (chunk.size() < 4)
    return;

  // Reference: decode_frame from libavcodec's h264.c

  if (chunk[0] == 0) { // Headers
    if (chunk.size() < 10 || !writeHeader)
      return;

    if (toc_)
      toc_->append(out_->realSize(), timestamp);
    int offset = 8;
    _nalLengthSize = (chunk[offset++] & 0x03) + 1;
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
      int len = (int)Bitwise::toUInt16(p, offset);
        offset += 2;
      if (offset + len > chunk.size()) break;
      write(_startCode, 0, _startCodeSize);
      write(chunk, offset, len);
      offset += len;
    }
  } else { // Video data
    if (toc_)
      toc_->append(out_->realSize(), timestamp);

    int offset = 4;

    if (_nalLengthSize != 2)
      _nalLengthSize = 4;

    const quint8 *p = (const quint8 *)chunk.data();
    while (offset <= chunk.size() - _nalLengthSize) {
      int len = (_nalLengthSize == 2) ?
        (int)Bitwise::toUInt16(p, offset) :
        (int)Bitwise::toUInt32(p, offset);
      offset += _nalLengthSize;
      if (offset + len > chunk.size()) break;
      write(_startCode, 0, _startCodeSize);
      write(chunk, offset, len);
      offset += len;
    }
  }
}

// EOF
