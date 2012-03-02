// settings.cc
// 7/30/2011

#include "settings.h"
#include "defines.h"
#include "mainwindow.h"
#include "module/annotcloud/traits.h"
#include "module/crypt/crypt.h"
#include "module/crypt/simplecrypt.h"
#include <QtCore>

// - Settings keys -

// See platform specific issue in QSettings manual.
#ifdef Q_OS_MAC
  #define SK_ORGANIZATION   G_DOMAIN
#else
  #define SK_ORGANIZATION   G_ORGANIZATION
#endif // Q_OS_MAC

#define SK_APPLICATION    G_APPLICATION
#define SK_VERSION        "Version"

#define SK_USERID       "UserId"
#define SK_USERNAME     "UserName"
#define SK_PASSWORD     "Password"

#define SK_ANNOTLANGUAGES "AnnotLanguages"
#define SK_LANGUAGE     "Language"
#define SK_LIVE         "Live"
#define SK_MENUBAR      "MenuBar"
#define SK_THEME        "Theme"
#define SK_TRANSLATE    "Translate"
#define SK_SUBTITLECOLOR "SubtitleColor"
#define SK_SUBTITLEONTOP "SubtitleOnTop"
#define SK_EMBEDONTOP   "EmbedOnTop"
#define SK_WINDOWONTOP  "WindowOnTop"
#define SK_UPDATEDATE   "UpdateDate"
#define SK_RECENTPATH   "RecentPath"
#define SK_AUTOPLAYNEXT "AutoPlayNext"
#define SK_ANNOTFILTER  "AnnotationFilter"
#define SK_ANNOTCOUNT   "AnnotationCount"
#define SK_ANNOTEFFECT  "AnnotationEffect"
#define SK_BLOCKEDUSERS "BlockedUsers"
#define SK_BLOCKEDKEYS  "BlockedKeywords"
#define SK_NICOACCOUNT  "NicovideoAccount"
#define SK_BILIACCOUNT  "BilibiliAccount"
#define SK_PLAYPOSHIST  "PlayPosHistory"
#define SK_SUBTITLEHIST "SubtitleHistory"
#define SK_TRACKHIST    "TrackHistory"

#define SK_QUEUEEMPTY   "QueueEmpty"

//#define SK_RECENT(_i)   "Recent" #_i
#define SK_RECENT       "Recent"

// - Helpers -

namespace { // anonymous

  template <typename T>
  inline QList<T>
  uniqueList(const QList<T> &l)
  {
    QList<T> ret;
    foreach (T t, l)
      if (!ret.contains(t))
        ret.append(t);
    return ret;
  }

} // anonymous namespace

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
Settings::setAnnotationEffect(int id)
{ setValue(SK_ANNOTEFFECT, id); }

int
Settings::annotationEffect() const
{
  bool ok;
  int ret = value(SK_ANNOTEFFECT).toInt(&ok);
  if (!ok)
    return 0;
  return ret;
}

void
Settings::setUserId(qint64 uid)
{ setValue(SK_USERID, uid); }

QString
Settings::userName() const
{
  QString ret = value(SK_USERNAME).toString();
  if (ret.isEmpty())
    return ret;
  else
    return Crypt::decrypt(ret);
}

void
Settings::setUserName(const QString &userName)
{
  if (userName.isEmpty())
    remove(SK_USERNAME);
  else
    setValue(SK_USERNAME, Crypt::encrypt(userName));
}

QString
Settings::password() const
{
  QString ret = value(SK_PASSWORD).toString();
  if (ret.isEmpty())
    return ret;
  else
    return Crypt::decrypt(ret);
}

void
Settings::setPassword(const QString &password)
{
  if (password.isEmpty())
    remove(SK_PASSWORD);
  else
    setValue(SK_PASSWORD, Crypt::encrypt(password));
}

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

int
Settings::subtitleColor() const
{
  enum { defval = MainWindow::Cyan };
  bool ok;
  int ret = value(SK_SUBTITLECOLOR, defval).toInt(&ok);
  if (!ok)
    ret = defval;
  return ret;
}

void
Settings::setSubtitleColor(int colorId)
{ setValue(SK_SUBTITLECOLOR, colorId); }

qint64
Settings::annotationLanguages() const
{
  enum { defval = AnnotCloud::Traits::AnyLanguageBit };
  bool ok;
  qint64 ret = value(SK_ANNOTLANGUAGES, defval).toLongLong(&ok);
  if (!ok)
    ret = defval;
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
Settings::isSubtitleOnTop() const
{ return value(SK_SUBTITLEONTOP).toBool(); }

void
Settings::setSubtitleOnTop(bool t)
{ setValue(SK_SUBTITLEONTOP, t); }

bool
Settings::isWindowOnTop() const
{ return value(SK_WINDOWONTOP, true).toBool(); }

void
Settings::setWindowOnTop(bool t)
{ setValue(SK_WINDOWONTOP, t); }

bool
Settings::isEmbeddedPlayerOnTop() const
{ return value(SK_EMBEDONTOP).toBool(); }

void
Settings::setEmbeddedPlayerOnTop(bool t)
{ setValue(SK_EMBEDONTOP, t); }

bool
Settings::isAnnotationFilterEnabled() const
{ return value(SK_ANNOTFILTER).toBool(); }

void
Settings::setAnnotationFilterEnabled(bool t)
{ setValue(SK_ANNOTFILTER, t); }

bool
Settings::isAutoPlayNext() const
{ return value(SK_AUTOPLAYNEXT, true).toBool(); }

void
Settings::setAutoPlayNext(bool t)
{ setValue(SK_AUTOPLAYNEXT, t); }

bool
Settings::isMenuBarVisible() const
{ return value(SK_MENUBAR).toBool(); }

void
Settings::setMenuBarVisible(bool t)
{ setValue(SK_MENUBAR, t); }

bool
Settings::isLive() const
{ return value(SK_LIVE).toBool(); }

void
Settings::setLive(bool t)
{ setValue(SK_LIVE, t); }

QString
Settings::recentPath() const
{ return value(SK_RECENTPATH).toString(); }

void
Settings::setRecentPath(const QString &path)
{ setValue(SK_RECENTPATH, path); }

QStringList
Settings::recentFiles() const
{
  QStringList ret = value(SK_RECENT).toStringList();

  //QString
  //r = value(SK_RECENT(0)).toString(); if (!r.isEmpty()) ret.append(r);
  //r = value(SK_RECENT(1)).toString(); if (!r.isEmpty()) ret.append(r);
  //r = value(SK_RECENT(2)).toString(); if (!r.isEmpty()) ret.append(r);
  //r = value(SK_RECENT(3)).toString(); if (!r.isEmpty()) ret.append(r);
  //r = value(SK_RECENT(4)).toString(); if (!r.isEmpty()) ret.append(r);
  //r = value(SK_RECENT(5)).toString(); if (!r.isEmpty()) ret.append(r);
  //r = value(SK_RECENT(6)).toString(); if (!r.isEmpty()) ret.append(r);
  //r = value(SK_RECENT(7)).toString(); if (!r.isEmpty()) ret.append(r);
  //r = value(SK_RECENT(8)).toString(); if (!r.isEmpty()) ret.append(r);
  //r = value(SK_RECENT(9)).toString(); if (!r.isEmpty()) ret.append(r);

  if (!ret.isEmpty())
    ret = ::uniqueList(ret);
  return ret;
}

//void
//Settings::clearRecentFiles()
//{
//  Q_ASSERT(RECENT_COUNT == G_RECENT_COUNT);
//  remove(SK_RECENT(0));
//  remove(SK_RECENT(1));
//  remove(SK_RECENT(2));
//  remove(SK_RECENT(3));
//  remove(SK_RECENT(4));
//  remove(SK_RECENT(5));
//  remove(SK_RECENT(6));
//  remove(SK_RECENT(7));
//  remove(SK_RECENT(8));
//  remove(SK_RECENT(9));
//}

void
Settings::setRecentFiles(const QStringList &l)
{
  if (l.isEmpty())
    remove(SK_RECENT);
  else
    setValue(SK_RECENT, l);

  //QString
  //r = l.size() <= 0 ? QString() : l[0]; setValue(SK_RECENT(0), r);
  //r = l.size() <= 1 ? QString() : l[1]; setValue(SK_RECENT(1), r);
  //r = l.size() <= 2 ? QString() : l[2]; setValue(SK_RECENT(2), r);
  //r = l.size() <= 3 ? QString() : l[3]; setValue(SK_RECENT(3), r);
  //r = l.size() <= 4 ? QString() : l[4]; setValue(SK_RECENT(4), r);
  //r = l.size() <= 5 ? QString() : l[5]; setValue(SK_RECENT(5), r);
  //r = l.size() <= 6 ? QString() : l[6]; setValue(SK_RECENT(6), r);
  //r = l.size() <= 7 ? QString() : l[7]; setValue(SK_RECENT(7), r);
  //r = l.size() <= 8 ? QString() : l[8]; setValue(SK_RECENT(8), r);
  //r = l.size() <= 9 ? QString() : l[9]; setValue(SK_RECENT(9), r);
}

void
Settings::setAnnotationCountHint(int v)
{ setValue(SK_ANNOTCOUNT, v); }

int
Settings::annotationCountHint() const
{ return value(SK_ANNOTCOUNT).toInt(); }

void
Settings::setBlockedKeywords(const QStringList &l)
{
  if (l.isEmpty())
    remove(SK_BLOCKEDKEYS);
  else
    setValue(SK_BLOCKEDKEYS, l);
}

QStringList
Settings::blockedKeywords() const
{ return value(SK_BLOCKEDKEYS).toStringList(); }

void
Settings::setBlockedUserNames(const QStringList &l)
{
  if (l.isEmpty())
    remove(SK_BLOCKEDUSERS);
  else
    setValue(SK_BLOCKEDUSERS, l);
}

QStringList
Settings::blockedUserNames() const
{ return value(SK_BLOCKEDUSERS).toStringList(); }

// - Accounts -

std::pair<QString, QString>
Settings::nicovideoAccount()
{
  QStringList v = value(SK_NICOACCOUNT).toStringList();
  if (v.size() != 2)
    return std::pair<QString, QString>();
  else {
    SimpleCrypt c(0);
    QString username = c.decryptToString(v.first());
    QString password = c.decryptToString(v.last());
    return std::make_pair(username, password);
  }
}

void
Settings::setNicovideoAccount(const QString &username, const QString &password)
{
  if (username.isEmpty() || password.isEmpty())
    remove(SK_NICOACCOUNT);
  else {
    SimpleCrypt c(0);
    QStringList v;
    v.append(c.encryptToString(username));
    v.append(c.encryptToString(password));
    setValue(SK_NICOACCOUNT, v);
  }
}

std::pair<QString, QString>
Settings::bilibiliAccount()
{
  QStringList v = value(SK_BILIACCOUNT).toStringList();
  if (v.size() != 2)
    return std::pair<QString, QString>();
  else {
    SimpleCrypt c(0);
    QString username = c.decryptToString(v.first());
    QString password = c.decryptToString(v.last());
    return std::make_pair(username, password);
  }
}

void
Settings::setBilibiliAccount(const QString &username, const QString &password)
{
  if (username.isEmpty() || password.isEmpty()) {
    remove(SK_BILIACCOUNT);
  } else {
    SimpleCrypt c(0);
    QStringList v;
    v.append(c.encryptToString(username));
    v.append(c.encryptToString(password));
    setValue(SK_BILIACCOUNT, v);
  }
}

// - Resume -

QHash<qint64, qint64>
Settings::playPosHistory() const
{
  QHash<qint64, qint64> ret;
  QHash<QString, QVariant> h = value(SK_PLAYPOSHIST).toHash();
  if (!h.isEmpty())
    foreach (QString k, h.keys())
      ret[k.toLongLong()] = h[k].toLongLong();
  return ret;
}

void
Settings::setPlayPosHistory(const QHash<qint64, qint64> &input)
{
  if (input.isEmpty())
    remove(SK_PLAYPOSHIST);
  else {
    QHash<QString, QVariant> h;
    foreach (qint64 k, input.keys())
      h[QString::number(k)] = QString::number(input[k]);
    setValue(SK_PLAYPOSHIST, h);
  }
}

QHash<qint64, int>
Settings::subtitleHistory() const
{
  QHash<qint64, int> ret;
  QHash<QString, QVariant> h = value(SK_SUBTITLEHIST).toHash();
  if (!h.isEmpty())
    foreach (QString k, h.keys())
      ret[k.toLongLong()] = h[k].toInt();
  return ret;
}

void
Settings::setSubtitleHistory(const QHash<qint64, int> &input)
{
  if (input.isEmpty())
    remove(SK_SUBTITLEHIST);
  else {
    QHash<QString, QVariant> h;
    foreach (qint64 k, input.keys())
      h[QString::number(k)] = QString::number(input[k]);
    setValue(SK_SUBTITLEHIST, h);
  }
}

QHash<qint64, int>
Settings::audioTrackHistory() const
{
  QHash<qint64, int> ret;
  QHash<QString, QVariant> h = value(SK_TRACKHIST).toHash();
  if (!h.isEmpty())
    foreach (QString k, h.keys())
      ret[k.toLongLong()] = h[k].toInt();
  return ret;
}

void
Settings::setAudioTrackHistory(const QHash<qint64, int> &input)
{
  if (input.isEmpty())
    remove(SK_TRACKHIST);
  else {
    QHash<QString, QVariant> h;
    foreach (qint64 k, input.keys())
      h[QString::number(k)] = QString::number(input[k]);
    setValue(SK_TRACKHIST, h);
  }
}

// EOF
