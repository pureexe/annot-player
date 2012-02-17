#ifndef AACCODEC_H
#define AACCODEC_H

// aaccodec.h
// 2/14/2011

#include <QtGlobal>

class InputStream;

struct AACInfo
{
  uint frequency; ///< such as 41000
  uint channels; ///< such as 0 or 2
  uint profile;
  quint8 config[2]; ///< config

  AACInfo() : frequency(0), channels(0), profile(0) { }
  bool isBad() const { return !frequency; }
};

class AACCodec
{
  AACCodec() { }

public:
  static AACInfo parseAACInfo(InputStream *in);
};

#endif // AACCODEC_H
