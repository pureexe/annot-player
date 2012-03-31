// settings.cc
// 7/30/2011

#include "settings.h"
#include "global.h"
#include "module/annotcloud/traits.h"
#include "module/crypt/crypt.h"
#include "module/crypt/simplecrypt.h"
#include "module/qtext/algorithm.h"
#include <QtCore>
#include <QNetworkProxy>

// - Settings keys -

// See platform specific issue in QSettings manual.
#ifdef Q_OS_MAC
  #define SK_ORGANIZATION   G_DOMAIN
#else
  #define SK_ORGANIZATION   G_ORGANIZATION
#endif // Q_OS_MAC

#define SK_APPLICATION  G_APPLICATION
#define SK_VERSION      "Version"

#define SK_RECENT       "Recent"

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

QStringList
Settings::recentUrls() const
{ return value(SK_RECENT).toStringList(); }

void
Settings::setRecentUrls(const QStringList &urls)
{
  if (urls.isEmpty())
    remove(SK_RECENT);
  else
    setValue(SK_RECENT, urls);
}

void
Settings::clearRecentUrls()
{ remove(SK_RECENT); }


// EOF
