// aacwriter.cc
// 2/12/2012

#include "module/mediacodec/mediawriter.h"
#include "module/mediacodec/mediatoc.h"
#include "module/qtext/bitwise.h"

#define DEBUG "aacwriter"
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
    quint64 bits = quint64(Bitwise::toUInt16(p, 1)) << 48;

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
      toc_->append(out_->availableSize(), timestamp);

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
    out_->write(bytes + 1, 7);
    out_->write(chunk.data() + 1, dataSize);
  }
}

// EOF
