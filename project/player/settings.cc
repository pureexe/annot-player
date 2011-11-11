// settings.cc
// 7/30/2011

#include "settings.h"
#include "global.h"
#include <QtCore>

// - Settings keys -

#define SK_ORGANIZATION   G_ORGANIZATION
#define SK_APPLICATION    G_APPLICATION
#define SK_VERSION        "Version"

#define SK_USERID       "UserId"
#define SK_USERNAME     "UserName"
#define SK_PASSWORD     "Password"

#define SK_ANNOTLANGUAGES "AnnotationLanguages"
#define SK_LANGUAGE     "Language"
#define SK_THEME        "Theme"
#define SK_TRANSLATE    "Translate"
#define SK_SUBTITLEONTOP "SubtitleStaysOnTop"
#define SK_UPDATEDATE   "UpdateDate"

#define SK_QUEUEEMPTY   "QueueEmpty"

#define SK_RECENT(_i)   "Recent" #_i
namespace { enum { RECENT_COUNT = 10 }; }

// - Constructions -

Settings*
Settings::globalInstance()
{ static Self global; return &global; }

Settings::Settings(QObject *parent)
  : Base(
      QSettings::NativeFormat, QSettings::UserScope,
      SK_ORGANIZATION, SK_APPLICATION,
      parent)
{  }

// - Properties -

QString
Settings::version() const
{ return value(SK_VERSION).toString(); }

void
Settings::setVersion(const QString &version)
{ setValue(SK_VERSION, version); }

qint64
Settings::userId() const
{
  bool ok;
  qint64 ret = value(SK_USERID).toLongLong(&ok);
  if (!ok)
    return 0;
  return ret;
}

void
Settings::setThemeId(int tid)
{ setValue(SK_THEME, tid); }

int
Settings::themeId() const
{
  bool ok;
  int ret = value(SK_THEME).toInt(&ok);
  if (!ok)
    return 0;
  return ret;
}


void
Settings::setUserId(qint64 uid)
{ setValue(SK_USERID, uid); }

QString
Settings::userName() const
{ return value(SK_USERNAME).toString(); }

void
Settings::setUserName(const QString &userName)
{ setValue(SK_USERNAME, userName); }

QString
Settings::password() const
{ return value(SK_PASSWORD).toString(); }

void
Settings::setPassword(const QString &password)
{ setValue(SK_PASSWORD, password); }

QDate
Settings::updateDate() const
{ return value(SK_UPDATEDATE).toDate(); }

void
Settings::setUpdateDate(const QDate &date)
{ setValue(SK_UPDATEDATE, date); }

int
Settings::language() const
{
  bool ok;
  int ret = value(SK_LANGUAGE).toInt(&ok);
  if (!ok)
    return 0;
  return ret;
}

void
Settings::setLanguage(int language)
{ setValue(SK_LANGUAGE, language); }

qint64
Settings::annotationLanguages() const
{
  bool ok;
  qint64 ret = value(SK_ANNOTLANGUAGES).toLongLong(&ok);
  if (!ok)
    return 0;
  return ret;
}

void
Settings::setAnnotationLanguages(qint64 bits)
{ setValue(SK_ANNOTLANGUAGES, bits); }


bool
Settings::isQueueEmpty() const
{ return value(SK_QUEUEEMPTY).toBool(); }

void
Settings::setQueueEmpty(bool empty)
{ setValue(SK_QUEUEEMPTY, empty); }

bool
Settings::isTranslateEnabled() const
{ return value(SK_TRANSLATE).toBool(); }

void
Settings::setTranslateEnabled(bool enabled)
{ setValue(SK_TRANSLATE, enabled); }

bool
Settings::isSubtitleStaysOnTop() const
{ return value(SK_SUBTITLEONTOP).toBool(); }

void
Settings::setSubtitleStaysOnTop(bool t)
{ setValue(SK_SUBTITLEONTOP, t); }


QStringList
Settings::recent() const
{
  QStringList ret;
  QString
  r = value(SK_RECENT(0)).toString(); if (!r.isEmpty()) ret.append(r);
  r = value(SK_RECENT(1)).toString(); if (!r.isEmpty()) ret.append(r);
  r = value(SK_RECENT(2)).toString(); if (!r.isEmpty()) ret.append(r);
  r = value(SK_RECENT(3)).toString(); if (!r.isEmpty()) ret.append(r);
  r = value(SK_RECENT(4)).toString(); if (!r.isEmpty()) ret.append(r);
  r = value(SK_RECENT(5)).toString(); if (!r.isEmpty()) ret.append(r);
  r = value(SK_RECENT(6)).toString(); if (!r.isEmpty()) ret.append(r);
  r = value(SK_RECENT(7)).toString(); if (!r.isEmpty()) ret.append(r);
  r = value(SK_RECENT(8)).toString(); if (!r.isEmpty()) ret.append(r);
  r = value(SK_RECENT(9)).toString(); if (!r.isEmpty()) ret.append(r);

  return ret;
}


void
Settings::clearRecent()
{
  setValue(SK_RECENT(0), QString());
  setValue(SK_RECENT(1), QString());
  setValue(SK_RECENT(2), QString());
  setValue(SK_RECENT(3), QString());
  setValue(SK_RECENT(4), QString());
  setValue(SK_RECENT(5), QString());
  setValue(SK_RECENT(6), QString());
  setValue(SK_RECENT(7), QString());
  setValue(SK_RECENT(8), QString());
  setValue(SK_RECENT(9), QString());
}

void
Settings::setRecent(const QStringList &l)
{
  if (l.isEmpty()) {
    clearRecent();
    return;
  }

  QString
  r = l.size() <= 0 ? QString() : l[0]; setValue(SK_RECENT(0), r);
  r = l.size() <= 1 ? QString() : l[1]; setValue(SK_RECENT(1), r);
  r = l.size() <= 2 ? QString() : l[2]; setValue(SK_RECENT(2), r);
  r = l.size() <= 3 ? QString() : l[3]; setValue(SK_RECENT(3), r);
  r = l.size() <= 4 ? QString() : l[4]; setValue(SK_RECENT(4), r);
  r = l.size() <= 5 ? QString() : l[5]; setValue(SK_RECENT(5), r);
  r = l.size() <= 6 ? QString() : l[6]; setValue(SK_RECENT(6), r);
  r = l.size() <= 7 ? QString() : l[7]; setValue(SK_RECENT(7), r);
  r = l.size() <= 8 ? QString() : l[8]; setValue(SK_RECENT(8), r);
  r = l.size() <= 9 ? QString() : l[9]; setValue(SK_RECENT(9), r);
}

// EOF
