// bitwise.h
// 2/12/2012
// See: FLVExtractCl/Library/General.cs
#ifndef _QTEXT_BITWISE_H
#define _QTEXT_BITWISE_H

namespace Bitwise {

  // - BitHelper -

  inline int read(quint64 &x, int length)
  {
    int r = (int)(x >> (64 - length));
    x <<= length;
    return r;
  }

  inline int read(const quint32 *bytes, int &offset, int length)
  {
    int startByte = offset / 8;
    int endByte = (offset + length - 1) / 8;
    int skipBits = offset % 8;
    quint64 bits = 0;
    for (int i = 0; i <= qMin(endByte - startByte, 7); i++) {
      bits |= (quint64)bytes[startByte + i] << (56 - (i * 8));
    }
    if (skipBits != 0)
      read(bits, skipBits);
    offset += length;
    return read(bits, length);
  }

  inline void write(quint64 &x, int length, int value)
  {
    quint64 mask = 0xFFFFFFFFFFFFFFFF >> (64 - length);
    x = (x << length) | ((quint64)value & mask);
  }

  // - BitConverterBE -
namespace BigEndian {

  inline quint64 toUInt64(const quint8 *value, int startIndex = 0)
  {
    return
      ((quint64)value[startIndex    ] << 56) |
      ((quint64)value[startIndex + 1] << 48) |
      ((quint64)value[startIndex + 2] << 40) |
      ((quint64)value[startIndex + 3] << 32) |
      ((quint64)value[startIndex + 4] << 24) |
      ((quint64)value[startIndex + 5] << 16) |
      ((quint64)value[startIndex + 6] <<  8) |
      ((quint64)value[startIndex + 7]      );
  }

  inline quint32 toUInt32(const quint8 *value, int startIndex = 0)
  {
    return
      ((quint32)value[startIndex    ] << 24) |
      ((quint32)value[startIndex + 1] << 16) |
      ((quint32)value[startIndex + 2] <<  8) |
      ((quint32)value[startIndex + 3]      );
  }

  inline quint16 toUInt16(const quint8 *value, int startIndex = 0)
  {
    return
      ((quint16)value[startIndex    ] <<  8) |
      ((quint16)value[startIndex + 1]      );
  }

  inline void getBytes(quint8 *buff, quint64 value)
  {
    buff[0] = (quint8)(value >> 56);
    buff[1] = (quint8)(value >> 48);
    buff[2] = (quint8)(value >> 40);
    buff[3] = (quint8)(value >> 32);
    buff[4] = (quint8)(value >> 24);
    buff[5] = (quint8)(value >> 16);
    buff[6] = (quint8)(value >>  8);
    buff[7] = (quint8)(value      );
  }

  inline void getBytes(quint8 *buff, quint32 value)
  {
    buff[0] = (quint8)(value >> 24);
    buff[1] = (quint8)(value >> 16);
    buff[2] = (quint8)(value >>  8);
    buff[3] = (quint8)(value      );
  }

  inline void getBytes(quint8 *buff, quint16 value)
  {
    buff[0] = (quint8)(value >>  8);
    buff[1] = (quint8)(value      );
  }

} // namespace BigEndian

  // - BitConverterLE -
namespace LittleEndian {
  inline void getBytes(quint8 *buff, quint64 value)
  {
    buff[0] = (quint8)(value      );
    buff[1] = (quint8)(value >>  8);
    buff[2] = (quint8)(value >> 16);
    buff[3] = (quint8)(value >> 24);
    buff[4] = (quint8)(value >> 32);
    buff[5] = (quint8)(value >> 40);
    buff[6] = (quint8)(value >> 48);
    buff[7] = (quint8)(value >> 56);
  }

  inline void getBytes(quint8 *buff, quint32 value)
  {
    buff[0] = (quint8)(value      );
    buff[1] = (quint8)(value >>  8);
    buff[2] = (quint8)(value >> 16);
    buff[3] = (quint8)(value >> 24);
  }

  inline void getBytes(quint8 *buff, quint16 value)
  {
    buff[0] = (quint8)(value      );
    buff[1] = (quint8)(value >>  8);
  }

} // namespace LittleEndian

} // namespace Bitwise

#endif // _QTEXT_BITWISE_H
