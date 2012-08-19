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

#define SK_LANG         "LanguageIndex"

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
Settings::languageIndex() const
{ return value(SK_LANG).toInt(); }

void
Settings::setLanguageIndex(int value)
{ setValue(SK_LANG, value); }

// EOF
