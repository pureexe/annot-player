// atlas.cc
// 9/6/2012

#include "atlas/atlas.h"
#include "atlas/atle_p.h"
#include "win/qtwin/qtwin.h"
#include "win/qtwin/winreg.h"
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtCore/QTextEncoder>
#include <QtCore/QTextDecoder>

// FIXME: mysterious error!
#ifdef __cplusplus
#ifdef QT_CORE_LIB
#include <QtCore/QSettings>

namespace WindowsRegistry {

  inline QVariant
  value(const QString &path, const QString &key, const QVariant &defval = QVariant())
  { return QSettings(path, QSettings::NativeFormat).value(key, defval); }

} // namespace WindowsRegistry

#endif // QT_CORE_LIB
#endif // __cplusplus

#define ATLE_REG_PATH   REG_HKLM_SOFTWARE "\\Fujitsu\\AtlEDict\\V14.0\\Env"
#define ATLE_REG_KEY    "DENJI"

#define DEBUG "atlas"
#include "module/debug/debug.h"

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
    ret = WindowsRegistry::value(ATLE_REG_PATH, ATLE_REG_KEY).toString();
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
