// core/util/textcodec.cc
// 7/18/2011

#include "core/util/textcodec.h"
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

// - Static helpers -

const char*
Core::
TextCodec::encoding(Type t)
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
Core::
TextCodec::description(Type t) {
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
Core::
TextCodec::encodings()
{
  return QStringList()
      << ENCODING_DEFAULT << ENCODING_UTF8 << ENCODING_SJIS << ENCODING_GBK;
}

QStringList
Core::
TextCodec::descriptions()
{
  return QStringList()
      << DESCRIPTION_DEFAULT << DESCRIPTION_UTF8 << DESCRIPTION_SJIS << DESCRIPTION_GBK;
}

// - Properties -

void
Core::
TextCodec::setEncoding(const QString &encoding)
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

// - Properties -

void
Core::
TextCodec::setType(Type type)
{
  if (type_ != type) {
    type_ = type;
    updateCodec();
  }
}

void
Core::
TextCodec::updateCodec()
{
  codec_ = type_ == Default?
      QTextCodec::codecForLocale() :
      QTextCodec::codecForName(Self::encoding(type_));

  decoder_ = codec_ ? codec_->makeDecoder() : 0;
}

// - Decoding -

QByteArray
Core::
TextCodec::encode(const QString &unicode) const
{
  if (codec_)
    return codec_->fromUnicode(unicode);
  else
    return unicode.toAscii();
}

// - Decoding -

QString
Core::
TextCodec::decode(const char *ascii) const
{
  if (type_ == UTF8)
    return QString::fromUtf8(ascii);
  else if (decoder_)
    return decoder_->toUnicode(ascii);
  else
    return ascii;
}

// EOF
