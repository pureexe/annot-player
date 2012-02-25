// mediawriter.cc
// 2/12/2012

#include "mediawriter.h"
#include "mediatoc.h"
#include "module/qtext/bitwise.h"
#include <QtCore>

#define DEBUG "audiowriter"
#include "module/debug/debug.h"

namespace Bitwise { using namespace BigEndian; }

// - AAC -

void
AacWriter::writeFrame(const QByteArray &chunk, quint32 timestamp, bool writeHeader)
{
  Q_UNUSED(writeHeader);
  if (chunk.size() < 1) return;

  if (chunk[0] == 0) { // Header
    if (chunk.size() < 3) return;

    const quint8 *p = (const quint8 *)chunk.data();
    quint64 bits = (quint64)Bitwise::toUInt16(p, 1) << 48;

    _aacProfile = Bitwise::read(bits, 5) - 1;
    _sampleRateIndex = Bitwise::read(bits, 4);
    _channelConfig = Bitwise::read(bits, 4);

    if ((_aacProfile < 0) || (_aacProfile > 3)) {
       DOUT("Unsupported AAC profile.");
       return;
    }
    if (_sampleRateIndex > 12) {
      DOUT("Invalid AAC sample rate index.");
      return;
    }
    if (_channelConfig > 6) {
      DOUT("Invalid AAC channel configuration.");
      return;
    }
  } else { // Audio data
    int dataSize = chunk.size() - 1;
    quint64 bits = 0;

    // Reference: WriteADTSHeader from FAAC's bitstream.c

    if (toc_)
      toc_->append(out_->realSize(), timestamp);

    Bitwise::write(bits, 12, 0xFFF);
    Bitwise::write(bits,  1, 0);
    Bitwise::write(bits,  2, 0);
    Bitwise::write(bits,  1, 1);
    Bitwise::write(bits,  2, _aacProfile);
    Bitwise::write(bits,  4, _sampleRateIndex);
    Bitwise::write(bits,  1, 0);
    Bitwise::write(bits,  3, _channelConfig);
    Bitwise::write(bits,  1, 0);
    Bitwise::write(bits,  1, 0);
    Bitwise::write(bits,  1, 0);
    Bitwise::write(bits,  1, 0);
    Bitwise::write(bits, 13, 7 + dataSize);
    Bitwise::write(bits, 11, 0x7FF);
    Bitwise::write(bits,  2, 0);

    quint8 bytes[8];
    Bitwise::getBytes(bytes, bits);
    write(bytes, 1, 7);
    write(chunk, 1, dataSize);
  }
}

// - H.264 -

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
