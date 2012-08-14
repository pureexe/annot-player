// settings.cc
// 7/30/2011

#include "settings.h"
#include "global.h"
#include "module/annotcloud/traits.h"
#include "module/crypt/crypt.h"
#include "module/crypt/simplecrypt.h"
#include "module/qtext/algorithm.h"
#include <QtCore>

// - Settings keys -

// See platform specific issue in QSettings manual.
#ifdef Q_OS_MAC
# define SK_ORGANIZATION G_DOMAIN
# define SK_APPLICATION  "downloader"
#else
# define SK_ORGANIZATION G_ORGANIZATION
# define SK_APPLICATION  G_APPLICATION
#endif // Q_OS_MAC

#define SK_VERSION      "Version"

#define SK_RECENT       "Recent"
#define SK_SIZE         "Size"

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

DownloadTaskInfoList
Settings::recentTasks() const
{
  DownloadTaskInfoList ret;
  QStringList v = value(SK_RECENT).toStringList();
  if (v.isEmpty() || v.size() % DownloadTaskInfo::FieldCount)
    return ret;

  DownloadTaskInfo t;
  for (int i = 0; i < v.size(); i++) {
    switch (i % DownloadTaskInfo::FieldCount) {
    case DownloadTaskInfo::StateField:   t.state = v[i].toInt(); break;
    case DownloadTaskInfo::UrlField:     t.url = v[i]; break;
    case DownloadTaskInfo::TitleField:   t.title = v[i]; break;
    case DownloadTaskInfo::FileNameField:t.fileName = v[i]; break;
    case DownloadTaskInfo::SizeField:    t.size = v[i].toLongLong();
                                         ret.append(t); break;
    default: Q_ASSERT(0);
    }
  }

  return ret;
}

void
Settings::setRecentTasks(const DownloadTaskInfoList &l)
{
  if (l.isEmpty())
    remove(SK_RECENT);
  else {
    QStringList v;
    foreach (const DownloadTaskInfo &t, l) {
      v.append(QString::number(t.state));
      v.append(t.url);
      v.append(t.title);
      v.append(t.fileName);
      v.append(QString::number(t.size));
    }
    setValue(SK_RECENT, v);
  }
}

QSize
Settings::recentSize() const
{ return value(SK_SIZE).toSize(); }

void
Settings::setRecentSize(const QSize &value)
{ setValue(SK_SIZE, value); }

// EOF
