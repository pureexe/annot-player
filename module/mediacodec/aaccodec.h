#ifndef AACCODEC_H
#define AACCODEC_H

// aaccodec.h
// 2/14/2011

#include <QtGlobal>

class InputStream;

struct AacInfo
{
  uint frequency; ///< such as 41000
  uint channels; ///< such as 0 or 2
  uint profile;
  quint8 config[2]; ///< config

  AacInfo() : frequency(0), channels(0), profile(0) { }
  bool isBad() const { return !frequency; }
};

class AacCodec
{
  AacCodec() { }

public:
  static AacInfo parseAacInfo(InputStream *in);
};

#endif // AACCODEC_H
