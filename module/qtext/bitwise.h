#ifndef _QTEXT_BITWISE_H
#define _QTEXT_BITWISE_H
// bitwise.h
// 2/12/2012
// See: FLVExtractCl/Library/General.cs

#include <QtGlobal>

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wstrict-aliasing" // dereferencing type-punned pointer
#endif // __GNUC__

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

  inline float toFloat(const quint8 *value, int startIndex = 0)
  {
    quint8 bytes[4];
    bytes[0] = value[startIndex + 3];
    bytes[1] = value[startIndex + 2];
    bytes[2] = value[startIndex + 1];
    bytes[3] = value[startIndex    ];
    return *(float *)bytes;
  }

  inline double toDouble(const quint8 *value, int startIndex = 0)
  {
    quint8 bytes[8];
    bytes[0] = value[startIndex + 7];
    bytes[1] = value[startIndex + 6];
    bytes[2] = value[startIndex + 5];
    bytes[3] = value[startIndex + 4];
    bytes[4] = value[startIndex + 3];
    bytes[5] = value[startIndex + 2];
    bytes[6] = value[startIndex + 1];
    bytes[7] = value[startIndex    ];
    return *(double *)bytes;
  }

  inline void getBytes(quint8 *buff, quint64 value)
  {
    buff[0] = value >> 56;
    buff[1] = value >> 48;
    buff[2] = value >> 40;
    buff[3] = value >> 32;
    buff[4] = value >> 24;
    buff[5] = value >> 16;
    buff[6] = value >>  8;
    buff[7] = value;
  }

  inline void getBytes(quint8 *buff, quint32 value)
  {
    buff[0] = value >> 24;
    buff[1] = value >> 16;
    buff[2] = value >>  8;
    buff[3] = value      ;
  }

  inline void getBytes(quint8 *buff, quint16 value)
  {
    buff[0] = value >>  8;
    buff[1] = value;
  }

  inline void getBytes(quint8 *buff, float value)
  {
    quint8 *p = (quint8 *)&value;
    buff[3] = *p++;
    buff[2] = *p++;
    buff[1] = *p++;
    buff[0] = *p++;
  }

  inline void getBytes(quint8 *buff, double value)
  {
    quint8 *p = (quint8 *)&value;
    buff[7] = *p++;
    buff[6] = *p++;
    buff[5] = *p++;
    buff[4] = *p++;
    buff[3] = *p++;
    buff[2] = *p++;
    buff[1] = *p++;
    buff[0] = *p;
  }

} // namespace BigEndian

  // - BitConverterLE -
namespace LittleEndian {

  inline float toFloat(const quint8 *value, int startIndex = 0)
  { return *(float *)(value + startIndex); }

  inline double toDouble(const quint8 *value, int startIndex = 0)
  { return *(double *)(value + startIndex); }

  inline void getBytes(quint8 *buff, quint64 value)
  {
    buff[0] = value;
    buff[1] = value >>  8;
    buff[2] = value >> 16;
    buff[3] = value >> 24;
    buff[4] = value >> 32;
    buff[5] = value >> 40;
    buff[6] = value >> 48;
    buff[7] = value >> 56;
  }

  inline void getBytes(quint8 *buff, quint32 value)
  {
    buff[0] = value;
    buff[1] = value >>  8;
    buff[2] = value >> 16;
    buff[3] = value >> 24;
  }

  inline void getBytes(quint8 *buff, quint16 value)
  {
    buff[0] = value;
    buff[1] = value >>  8;
  }

  inline void getBytes(quint8 *buff, float value)
  {
    quint8 *p = (quint8 *)&value;
    buff[0] = *p++;
    buff[1] = *p++;
    buff[2] = *p++;
    buff[3] = *p++;
  }

  inline void getBytes(quint8 *buff, double value)
  {
    quint8 *p = (quint8 *)&value;
    buff[0] = *p++;
    buff[1] = *p++;
    buff[2] = *p++;
    buff[3] = *p++;
    buff[4] = *p++;
    buff[5] = *p++;
    buff[6] = *p++;
    buff[7] = *p++;
  }
} // namespace LittleEndian

} // namespace Bitwise

#endif // _QTEXT_BITWISE_H
