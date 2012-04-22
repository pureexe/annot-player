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
#  define SK_ORGANIZATION   G_DOMAIN
#else
#  define SK_ORGANIZATION   G_ORGANIZATION
#endif // Q_OS_MAC

#define SK_APPLICATION  G_APPLICATION
#define SK_VERSION      "Version"

#define SK_SIZE         "Size"
#define SK_RECENT       "Recent"
#define SK_RECENTTABS   "Tabs"
#define SK_RECENTTABINDEX "TabIndex"
#define SK_RECENTCLOSED "Closed"

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

// - History -

QSize
Settings::recentSize() const
{ return value(SK_SIZE).toSize(); }

void
Settings::setRecentSize(const QSize &value)
{ setValue(SK_SIZE, value); }

QStringList
Settings::recentUrls() const
{ return value(SK_RECENT).toStringList(); }

void
Settings::setRecentUrls(const QStringList &urls, int limit)
{
  if (urls.isEmpty())
    remove(SK_RECENT);
  else {
    if (!limit || urls.size() <= limit)
      setValue(SK_RECENT, urls);
    else {
      QStringList l = urls;
      while (l.size() > limit)
        l.removeLast();
      setValue(SK_RECENT, l);
    }
  }
}

void
Settings::clearRecentUrls()
{ remove(SK_RECENT); }

QStringList
Settings::recentTabs() const
{ return value(SK_RECENTTABS).toStringList(); }

bool
Settings::hasRecentTabs() const
{ return value(SK_RECENTTABS).isValid(); }

void
Settings::setRecentTabs(const QStringList &urls, int limit)
{
  if (urls.isEmpty())
    remove(SK_RECENTTABS);
  else {
    if (!limit || urls.size() <= limit)
      setValue(SK_RECENTTABS, urls);
    else {
      QStringList l = urls;
      while (l.size() > limit)
        l.removeLast();
      setValue(SK_RECENTTABS, l);
    }
  }
}

void
Settings::clearRecentTabs()
{ remove(SK_RECENTTABS); }

QList<QUrl>
Settings::closedUrls() const
{
  QList<QUrl> ret;
  foreach (QVariant v, value(SK_RECENTCLOSED).toList()) {
    QUrl url = v.toUrl();
    if (!url.isEmpty())
      ret.append(url);
  }
  return ret;
}

void
Settings::setClosedUrls(const QList<QUrl> &urls, int limit)
{
  if (urls.isEmpty())
    remove(SK_RECENTCLOSED);
  else {
    QList<QVariant> value;
    int size = qMin(limit, urls.size());
    for (int i = 0; i < size; i++)
      value.append(urls[i]);
    setValue(SK_RECENTCLOSED, value);
  }
}

void
Settings::clearClosedUrls()
{ remove(SK_RECENTCLOSED); }

int
Settings::recentTabIndex() const
{ return value(SK_RECENTTABINDEX).toInt(); }

void
Settings::setRecentTabIndex(int value)
{ setValue(SK_RECENTTABINDEX, value); }

// EOF
