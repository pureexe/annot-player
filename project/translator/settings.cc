// settings.cc
// 7/30/2011

#include "settings.h"
#include "global.h"
#include <QtCore>

// - Settings keys -

// See platform specific issue in QSettings manual.
#ifdef Q_OS_MAC
# define SK_ORGANIZATION G_DOMAIN
# define SK_APPLICATION  "translator"
#else
# define SK_ORGANIZATION G_ORGANIZATION
# define SK_APPLICATION  G_APPLICATION
#endif // Q_OS_MAC

#define SK_VERSION      "Version"

#define SK_RECENT       "Recent"
#define SK_SIZE         "Size"

#define SK_LANG         "Language"
#define SK_DICT         "Dictionary"
#define SK_TOP          "Top"
#define SK_CLIPBOARD    "Clipboard"
#define SK_TRANSLATOR   "Translator"
#define SK_ATLAS        "Atlas"

// - Constructions -

Settings::Settings(QObject *parent)
  : Base(
      QSettings::NativeFormat, QSettings::UserScope,
      SK_ORGANIZATION, SK_APPLICATION,
      parent)
{ }

// - Properties -

QString
Settings::version() const
{ return value(SK_VERSION).toString(); }

void
Settings::setVersion(const QString &version)
{ setValue(SK_VERSION, version); }

// - Recent -

QSize
Settings::recentSize() const
{ return value(SK_SIZE).toSize(); }

void
Settings::setRecentSize(const QSize &value)
{ setValue(SK_SIZE, value); }

int
Settings::language() const
{ return value(SK_LANG).toInt(); }

void
Settings::setLanguage(int value)
{ setValue(SK_LANG, value); }
int

Settings::dictionary() const
{ return value(SK_DICT).toInt(); }

void
Settings::setDictionary(int value)
{ setValue(SK_DICT, value); }

bool
Settings::monitorClipboard() const
{ return value(SK_CLIPBOARD).toBool(); }

void
Settings::setMonitorClipboard(bool value)
{ setValue(SK_CLIPBOARD, value); }

bool
Settings::windowOnTop() const
{ return value(SK_TOP, true).toBool(); }

void
Settings::setWindowOnTop(bool value)
{ setValue(SK_TOP, value); }

void
Settings::setTranslationServices(ulong value)
{ setValue(SK_TRANSLATOR, uint(value)); }

ulong
Settings::translationServices() const
{ return value(SK_TRANSLATOR).toUInt(); }

bool
Settings::isAtlasEnabled() const
{ return value(SK_ATLAS, true).toBool(); }

void
Settings::setAtlasEnabled(bool value)
{ setValue(SK_ATLAS, value); }

// EOF
