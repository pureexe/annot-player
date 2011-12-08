// core/util/codec.cc
// 7/18/2011

#include "core/util/codec.h"
#include <QtCore>

// http://doc.qt.nokia.com/latest/qtextcodec.html
#define ENCODING_UTF8   "UTF-8"
#define ENCODING_DEFAULT "Default"
#define ENCODING_SJIS   "SHIFT-JIS"
#define ENCODING_GBK    "GBK"

#define DESCRIPTION_UTF8        "Apple"
#define DESCRIPTION_DEFAULT     "Windows"
#define DESCRIPTION_SJIS        "Japanese"
#define DESCRIPTION_GBK         "Chinese"
namespace Core {

  class CodecImpl
  {
    QTextCodec *codec_;
    QTextDecoder *decoder_;

  public:
    CodecImpl() : codec_(0), decoder_(0) { }

    QTextCodec *codec() const { return codec_; }
    void setCodec(QTextCodec *codec) { codec_ = codec; }

    QTextDecoder *decoder() const { return decoder_; }
    void setDecoder(QTextDecoder *decoder) { decoder_ = decoder; }
  };
} // namespace Core

namespace Core {

  // - Static helpers -
  const char*
  Codec::encoding(Type t)
  {
    switch (t) {
    case Default:       return ENCODING_DEFAULT;
    case UTF8:          return ENCODING_UTF8;
    case SJIS:          return ENCODING_SJIS;
    case GBK:           return ENCODING_GBK;

    default:
      Q_ASSERT(0);
      return 0;
    }
  }

  QString
  Codec::description(Type t) {
    switch (t) {
    case Default:       return DESCRIPTION_DEFAULT;
    case UTF8:          return DESCRIPTION_UTF8;
    case SJIS:          return DESCRIPTION_SJIS;
    case GBK:           return DESCRIPTION_GBK;

    default:
      Q_ASSERT(0);
      return QString();
    }
  }

  QStringList
  Codec::encodings()
  {
    return QStringList()
        << ENCODING_DEFAULT << ENCODING_UTF8 << ENCODING_SJIS << ENCODING_GBK;
  }

  QStringList
  Codec::descriptions()
  {
    return QStringList()
        << DESCRIPTION_DEFAULT << DESCRIPTION_UTF8 << DESCRIPTION_SJIS << DESCRIPTION_GBK;
  }

  // - Constructions -
  Codec::Codec(const QString &encoding, QObject *parent)
    : Base(parent), type_(Default), impl_(0)
  {
    setEncoding(encoding);

    QTextCodec *codec = type() == Default?
          QTextCodec::codecForLocale()
        : QTextCodec::codecForName(Self::encoding(type_));

    if (!codec)
      return;

    QTextDecoder *decoder = codec->makeDecoder();
    if (!decoder)
      return;

    impl_ = new Impl;
    impl_->setCodec(codec);
    impl_->setDecoder(decoder);
  }

  Codec::~Codec()
  { if (impl_) delete impl_; }

  // - Properties -
  void
  Codec::setEncoding(const QString &encoding)
  {
    if (encoding == ENCODING_DEFAULT)
      setType(Default);
    else if (encoding == ENCODING_UTF8)
      setType(UTF8);
    else if (encoding == ENCODING_SJIS)
      setType(SJIS);
    else if (encoding == ENCODING_GBK)
      setType(GBK);
    else {
      Q_ASSERT(0);
      setType(Default);
    }
  }

  // - Decoding -
  QByteArray
  Codec::encode(const QString &unicode) const
  {
    if (!valid())
      return unicode.toAscii();
    else
      return impl_->codec()->fromUnicode(unicode);
  }

  // - Decoding -
  QString
  Codec::decode(const char *ascii) const
  {
    if (type() == UTF8)
      return QString::fromUtf8(ascii);
    else if (!valid())
      return ascii;
    else
      return impl_->decoder()->toUnicode(ascii);
  }

} // namespace Core

// EOF
