// aacwriter.cc
// 2/12/2012

#include "lib/mediacodec/mediawriter.h"
#include "lib/mediacodec/mediatoc.h"
#include "qtx/qxbitwise.h"

#define DEBUG "aacwriter"
#include "qtx/qxdebug.h"

namespace QxBitwise { using namespace BigEndian; }

// - AAC -

void
AacWriter::writeFrame(const QByteArray &chunk, quint32 timestamp, bool writeHeader)
{
  Q_UNUSED(writeHeader)
  if (chunk.size() < 1) return;

  if (chunk[0] == 0) { // Header
    if (chunk.size() < 3) return;

    const quint8 *p = (const quint8 *)chunk.data();
    quint64 bits = quint64(QxBitwise::toUInt16(p, 1)) << 48;

    _aacProfile = QxBitwise::read(bits, 5) - 1;
    _sampleRateIndex = QxBitwise::read(bits, 4);
    _channelConfig = QxBitwise::read(bits, 4);

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

    QxBitwise::write(bits, 12, 0xFFF);
    QxBitwise::write(bits,  1, 0);
    QxBitwise::write(bits,  2, 0);
    QxBitwise::write(bits,  1, 1);
    QxBitwise::write(bits,  2, _aacProfile);
    QxBitwise::write(bits,  4, _sampleRateIndex);
    QxBitwise::write(bits,  1, 0);
    QxBitwise::write(bits,  3, _channelConfig);
    QxBitwise::write(bits,  1, 0);
    QxBitwise::write(bits,  1, 0);
    QxBitwise::write(bits,  1, 0);
    QxBitwise::write(bits,  1, 0);
    QxBitwise::write(bits, 13, 7 + dataSize);
    QxBitwise::write(bits, 11, 0x7FF);
    QxBitwise::write(bits,  2, 0);

    quint8 bytes[8];
    QxBitwise::getBytes(bytes, bits);
    out_->write(bytes + 1, 7);
    out_->write(chunk.data() + 1, dataSize);
  }
}

// EOF
