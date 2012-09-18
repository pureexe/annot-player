// atlas.cc
// 9/6/2012

#include "atlas/atlas.h"
#include "atlas/atle_p.h"
#include "win/qtwin/qtwin.h"
#include "win/reg/regdefs.h"
#include "win/reg/regio.h"
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QTextEncoder>
#include <QtCore/QTextDecoder>
#include <QtCore/QSettings>

#define ATLE_REG_PATH   REG_HKLM_SOFTWARE "\\Fujitsu\\AtlEDict\\V14.0\\Env"
#define ATLE_REG_KEY    "DENJI"

#define DEBUG "atlas"
#include "qtx/qxdebug.h"

// - Construction -

Atlas::Atlas(QObject *parent)
  : Base(parent), atle_(0)
{
  QTextCodec *codec = QTextCodec::codecForName(ATLE_ENCODING);
  Q_ASSERT(codec);
  decoder_ = codec->makeDecoder();
  Q_ASSERT(decoder_);
  encoder_ = codec->makeEncoder();
  Q_ASSERT(encoder_);
}

QString
Atlas::decodeText(const QByteArray &data) const
{ return decoder_->toUnicode(data); }

QByteArray
Atlas::encodeText(const QString &data) const
{ return encoder_->fromUnicode(data); }

// - Location -

bool
Atlas::isValidLocation(const QString &path)
{ return QFile::exists(path + '/' + ATLE_DLL ".dll"); }

QString
Atlas::findLocation()
{
  QString ret = QtWin::getProgramFilesPath() + "/" "ATLAS V14";

  if (!isValidLocation(ret))
    ret = RegIO::readValue(ATLE_REG_PATH, ATLE_REG_KEY).toString();
  DOUT("ret =" << ret);
  return ret;
}

void
Atlas::setLocation(const QString &path)
{
  if (location_ != path) {
    location_ = path;
    if (!location_.isEmpty())
      QtWin::appendPath(path);
    emit locationChanged(location_);
  }
}

// - Translate -

QString
Atlas::translate(const QString &text)
{
  if (text.isEmpty())
    return QString();

  Atle *e = atle();
  if (!e->isLoaded())
    return QString();

  return decodeText(e->translate(encodeText(text)));
}

// - ATLE -

Atle*
Atlas::atle()
{
  if (!atle_) {
    atle_ = new Atle(this);
    atle_->load();

  }
  return atle_;
}

// EOF
