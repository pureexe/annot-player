// settings.cc
// 7/30/2011

#include "settings.h"
#include "global.h"
#include "annotationsettings.h"
#include "module/annotcloud/traits.h"
#include "module/qtext/algorithm.h"
#include <QtCore>

#define DEBUG "settings"
#include "module/debug/debug.h"

using namespace AnnotCloud;

// - Settings keys -

// See platform specific issue in QSettings manual.
#ifdef Q_OS_MAC
# define SK_ORGANIZATION  G_DOMAIN
# define SK_APPLICATION   "player"
#else
# define SK_ORGANIZATION  G_ORGANIZATION
# define SK_APPLICATION   G_APPLICATION
#endif // Q_OS_MAC

#define SK_VERSION        "Version"
#define SK_PATH           "Path"

#define SK_ANNOTLANGUAGES "AnnotLanguages"
#define SK_LIVE         "Live"
#define SK_PREFERLOCALDB  "PreferLocalDatabase"
#define SK_MENUBAR      "MenuBar"
#define SK_TRANSLATE    "Translate"
#define SK_SUBTITLECOLOR "SubtitleColor"
#define SK_HIGHLIGHTCOLOR "AnnotationHighlightColor"
#define SK_SUBTITLEONTOP "SubtitleOnTop"
#define SK_SUBOUTCOLOR  "SubtitleOutline"
#define SK_ANNOTOUTCOLOR  "AnnotationOutline"
#define SK_EMBEDONTOP   "EmbedOnTop"
#define SK_LABELPLAYER  "LabelPlayer"
#define SK_SAVEBUFFER   "SaveBuffer"
#define SK_SAVEANNOT    "SaveAnnotation"
#define SK_WINDOWONTOP  "WindowOnTop"
#define SK_UPDATEDATE   "UpdateDate"
#define SK_RECENTPATH   "RecentPath"
#define SK_GAMEENCODINGS "GameEncodings"
#define SK_AUTOSUBMIT   "Submit"
#define SK_ANNOTBANDWIDTH   "AnnotationBandwidth"
#define SK_ANNOTAVATAR  "AnnotationAvatar"
#define SK_ANNOTMETA  "AnnotationMeta"
#define SK_ANNOTSCALE   "AnnotationScale"
//#define SK_ANNOTROTATE  "AnnotationRotation"
#define SK_ANNOTRESOLUTION "AnnotationResolution"
#define SK_ANNOTFILTER  "AnnotationFilter"
#define SK_ANNOTCOUNT   "AnnotationCount"
#define SK_ANNOTEFFECT  "AnnotationEffect"
#define SK_ANNOTOFFSET  "AnnotationOffset"
#define SK_ANNOTFONT    "AnnotationFont"
#define SK_ANNOTJAPANFONT "AnnotationJapaneseFont"
#define SK_ANNOTCHINAFONT "AnnotationChineseFont"
#define SK_ANNOTZHT     "TraditionalChinese"
#define SK_MOTIONLESS   "Motionless"
#define SK_AUTOPLAYNEXT "AutoPlayNext"
#define SK_BLOCKEDUSERS "BlockedUsers"
#define SK_BLOCKEDKEYS  "BlockedKeywords"
#define SK_PLAYPOSHIST  "PlayPosHistory"
#define SK_SUBTITLEHIST "SubtitleHistory"
#define SK_TRACKHIST    "TrackHistory"
#define SK_CHANNELHIST  "ChannelHistory"
#define SK_ASPECTHIST   "AspectHistory"
#define SK_MULTIWINDOW  "MultipleWindows"
#define SK_QUEUEEMPTY   "QueueEmpty"
#define SK_RECENT       "Recent"
#define SK_RECENTTITLES "RecentTitles"
#define SK_HUE          "Hue"
#define SK_SATURATION   "Saturation"
#define SK_BRIGHTNESS   "Brightness"
#define SK_CONTRAST     "Contrast"
#define SK_GAMMA        "Gamma"

// - Constructions -

Settings::Settings(QObject *parent)
  : Base(
      Base::NativeFormat, Base::UserScope,
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

QString
Settings::applicationFilePath() const
{ return value(SK_PATH).toString(); }

void
Settings::setApplicationFilePath(const QString &path)
{ setValue(SK_PATH, path); }

void
Settings::setAnnotationEffect(int id)
{ setValue(SK_ANNOTEFFECT, id); }

int
Settings::annotationEffect() const
{ return value(SK_ANNOTEFFECT).toInt(); }

void
Settings::setAnnotationPositionResolution(int value)
{ setValue(SK_ANNOTRESOLUTION, value); }

int
Settings::annotationPositionResolution() const
{ return value(SK_ANNOTRESOLUTION, ANNOTATION_POSITION_RESOLUTION).toInt(); }

void
Settings::setAnnotationOffset(qint64 offset)
{ setValue(SK_ANNOTOFFSET, offset); }

qint64
Settings::annotationOffset() const
{ return value(SK_ANNOTOFFSET).toLongLong(); }

void
Settings::setPreferMotionlessAnnotation(bool t)
{ setValue(SK_MOTIONLESS, t); }

bool
Settings::preferMotionlessAnnotation() const
{ return value(SK_MOTIONLESS, true).toBool(); }

void
Settings::setPreferTraditionalChinese(bool t)
{ setValue(SK_ANNOTZHT, t); }

bool
Settings::preferTraditionalChinese() const
{ return value(SK_ANNOTZHT).toBool(); }


void
Settings::setAnnotationFontFamily(const QString &family)
{ setValue(SK_ANNOTFONT, family); }

QString
Settings::annotationFontFamily() const
{ return value(SK_ANNOTFONT).toString(); }

void
Settings::setAnnotationJapaneseFontFamily(const QString &family)
{ setValue(SK_ANNOTJAPANFONT, family); }

QString
Settings::annotationJapaneseFontFamily() const
{ return value(SK_ANNOTJAPANFONT).toString(); }

void
Settings::setAnnotationChineseFontFamily(const QString &family)
{ setValue(SK_ANNOTCHINAFONT, family); }

QString
Settings::annotationChineseFontFamily() const
{ return value(SK_ANNOTCHINAFONT).toString(); }

QDate
Settings::updateDate() const
{ return value(SK_UPDATEDATE).toDate(); }

void
Settings::setUpdateDate(const QDate &date)
{ setValue(SK_UPDATEDATE, date); }

int
Settings::subtitleColor() const
{ return value(SK_SUBTITLECOLOR).toInt(); }

void
Settings::setSubtitleColor(int colorId)
{ setValue(SK_SUBTITLECOLOR, colorId); }

qint64
Settings::annotationLanguages() const
{
  enum { defval = Traits::AllLanguages };
  return value(SK_ANNOTLANGUAGES, defval).toLongLong();
}

void
Settings::setAnnotationLanguages(qint64 bits)
{ setValue(SK_ANNOTLANGUAGES, bits); }

QColor
Settings::annotationHighlightColor() const
{ return value(SK_HIGHLIGHTCOLOR).value<QColor>(); }

void
Settings::setAnnotationHighlightColor(const QColor &value)
{ setValue(SK_HIGHLIGHTCOLOR, value); }

QColor
Settings::annotationOutlineColor() const
{ return value(SK_ANNOTOUTCOLOR).value<QColor>(); }

void
Settings::setAnnotationOutlineColor(const QColor &value)
{ setValue(SK_ANNOTOUTCOLOR, value); }

QColor
Settings::subtitleOutlineColor() const
{ return value(SK_SUBOUTCOLOR).value<QColor>(); }

void
Settings::setSubtitleOutlineColor(const QColor &value)
{ setValue(SK_SUBOUTCOLOR, value); }

bool
Settings::isMultipleWindowsEnabled() const
{ return value(SK_MULTIWINDOW).toBool(); }

void
Settings::setMultipleWindowsEnabled(bool t)
{ setValue(SK_MULTIWINDOW, t); }

bool
Settings::isQueueEmpty() const
{ return value(SK_QUEUEEMPTY).toBool(); }

void
Settings::setQueueEmpty(bool empty)
{ setValue(SK_QUEUEEMPTY, empty); }

bool
Settings::isBufferedMediaSaved() const
{ return value(SK_SAVEBUFFER, true).toBool(); }

void
Settings::setBufferedMediaSaved(bool t)
{ setValue(SK_SAVEBUFFER, t); }

bool
Settings::isAnnotationFileSaved() const
{ return value(SK_SAVEANNOT, true).toBool(); }

void
Settings::setAnnotationFileSaved(bool t)
{ setValue(SK_SAVEANNOT, t); }

bool
Settings::isAutoSubmit() const
{ return value(SK_AUTOSUBMIT, true).toBool(); }

void
Settings::setAutoSubmit(bool t)
{ setValue(SK_AUTOSUBMIT, t); }

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
{ return value(SK_WINDOWONTOP).toBool(); }

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
Settings::isPlayerLabelEnabled() const
{ return value(SK_LABELPLAYER, true).toBool(); }

void
Settings::setPlayerLabelEnabled(bool t)
{ setValue(SK_LABELPLAYER, t); }

bool
Settings::isAnnotationAvatarVisible() const
{ return value(SK_ANNOTAVATAR, true).toBool(); }

void
Settings::setAnnotationAvatarVisible(bool t)
{ setValue(SK_ANNOTAVATAR, t); }

bool
Settings::isAnnotationMetaVisible() const
{ return value(SK_ANNOTMETA).toBool(); }

void
Settings::setAnnotationMetaVisible(bool t)
{ setValue(SK_ANNOTMETA, t); }

bool
Settings::isAnnotationBandwidthLimited() const
{ return value(SK_ANNOTBANDWIDTH, true).toBool(); }

void
Settings::setAnnotationBandwidthLimited(bool t)
{ setValue(SK_ANNOTBANDWIDTH, t); }

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

bool
Settings::preferLocalDatabase() const
{ return value(SK_PREFERLOCALDB, true).toBool(); }

void
Settings::setPreferLocalDatabase(bool t)
{ setValue(SK_PREFERLOCALDB, t); }

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
    ret = QtExt::uniqueList(ret);
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

QHash<QString, QString>
Settings::recentTitles() const
{
  QHash<QString, QString> ret;
  QHash<QString, QVariant> h = value(SK_RECENTTITLES).toHash();
  if (!h.isEmpty())
    for (auto p = h.begin(); p != h.end(); ++p)
      ret[p.key()] = p.value().toString();
  return ret;
}

void
Settings::setRecentTitles(const QHash<QString, QString> &input)
{
  if (input.isEmpty())
    remove(SK_RECENTTITLES);
  else {
    QHash<QString, QVariant> h;
    for (auto p = input.begin(); p != input.end(); ++p)
      h[p.key()] = p.value();
    setValue(SK_RECENTTITLES, h);
  }
}

QHash<QString, QString>
Settings::gameEncodings() const
{
  QHash<QString, QString> ret;
  QHash<QString, QVariant> h = value(SK_GAMEENCODINGS).toHash();
  if (!h.isEmpty())
    for (auto p = h.begin(); p != h.end(); ++p)
      ret[p.key()] = p.value().toString();
  return ret;
}

void
Settings::setGameEncodings(const QHash<QString, QString> &input, int limit)
{
  if (input.isEmpty())
    remove(SK_GAMEENCODINGS);
  else {
    QHash<QString, QVariant> h;
    int count = 0;
    for (auto p = input.begin(); p != input.end(); ++p) {
      if (limit && count++ >= limit)
        break;
      h[p.key()] = p.value();
    }
    setValue(SK_GAMEENCODINGS, h);
  }
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

// - Resume -

QHash<qint64, qint64>
Settings::playPosHistory() const
{
  QHash<qint64, qint64> ret;
  QHash<QString, QVariant> h = value(SK_PLAYPOSHIST).toHash();
  if (!h.isEmpty())
    for (auto p = h.begin(); p != h.end(); ++p)
      ret[p.key().toLongLong()] = p.value().toLongLong();
  return ret;
}

void
Settings::setPlayPosHistory(const QHash<qint64, qint64> &input)
{
  if (input.isEmpty())
    remove(SK_PLAYPOSHIST);
  else {
    QHash<QString, QVariant> h;
    for (auto p = input.begin(); p != input.end(); ++p)
      h[QString::number(p.key())] = p.value();
    setValue(SK_PLAYPOSHIST, h);
  }
}

QHash<qint64, int>
Settings::subtitleHistory() const
{
  QHash<qint64, int> ret;
  QHash<QString, QVariant> h = value(SK_SUBTITLEHIST).toHash();
  if (!h.isEmpty())
    for (auto p = h.begin(); p != h.end(); ++p)
      ret[p.key().toLongLong()] = p.value().toInt();
  return ret;
}

void
Settings::setSubtitleHistory(const QHash<qint64, int> &input)
{
  if (input.isEmpty())
    remove(SK_SUBTITLEHIST);
  else {
    QHash<QString, QVariant> h;
    for (auto p = input.begin(); p != input.end(); ++p)
      h[QString::number(p.key())] = p.value();
    setValue(SK_SUBTITLEHIST, h);
  }
}

QHash<qint64, int>
Settings::audioTrackHistory() const
{
  QHash<qint64, int> ret;
  QHash<QString, QVariant> h = value(SK_TRACKHIST).toHash();
  if (!h.isEmpty())
    for (auto p = h.begin(); p != h.end(); ++p)
      ret[p.key().toLongLong()] = p.value().toInt();
  return ret;
}

void
Settings::setAudioTrackHistory(const QHash<qint64, int> &input)
{
  if (input.isEmpty())
    remove(SK_TRACKHIST);
  else {
    QHash<QString, QVariant> h;
    for (auto p = input.begin(); p != input.end(); ++p)
      h[QString::number(p.key())] = p.value();
    setValue(SK_TRACKHIST, h);
  }
}

QHash<qint64, int>
Settings::audioChannelHistory() const
{
  QHash<qint64, int> ret;
  QHash<QString, QVariant> h = value(SK_CHANNELHIST).toHash();
  if (!h.isEmpty())
    for (auto p = h.begin(); p != h.end(); ++p)
      ret[p.key().toLongLong()] = p.value().toInt();
  return ret;
}

void
Settings::setAudioChannelHistory(const QHash<qint64, int> &input)
{
  if (input.isEmpty())
    remove(SK_CHANNELHIST);
  else {
    QHash<QString, QVariant> h;
    for (auto p = input.begin(); p != input.end(); ++p)
      h[QString::number(p.key())] = p.value();
    setValue(SK_CHANNELHIST, h);
  }
}

QHash<qint64, QString>
Settings::aspectRatioHistory() const
{
  QHash<qint64, QString> ret;
  QHash<QString, QVariant> h = value(SK_ASPECTHIST).toHash();
  if (!h.isEmpty())
    for (auto p = h.begin(); p != h.end(); ++p)
      ret[p.key().toLongLong()] = p.value().toString();
  return ret;
}

void
Settings::setAspectRatioHistory(const QHash<qint64, QString> &input)
{
  if (input.isEmpty())
    remove(SK_PLAYPOSHIST);
  else {
    QHash<QString, QVariant> h;
    for (auto p = input.begin(); p != input.end(); ++p)
      h[QString::number(p.key())] = p.value();
    setValue(SK_ASPECTHIST, h);
  }
}

// - Video control -

void
Settings::setHue(int v)
{ setValue(SK_HUE, v); }

int
Settings::hue() const
{ return value(SK_HUE).toInt(); }

void
Settings::setGamma(qreal v)
{ setValue(SK_GAMMA, v); }

qreal
Settings::gamma() const
{ return value(SK_GAMMA, 1.0).toReal(); }

void
Settings::setContrast(qreal v)
{ setValue(SK_CONTRAST, v); }

qreal
Settings::contrast() const
{ return value(SK_CONTRAST, 1.0).toReal(); }

void
Settings::setSaturation(qreal v)
{ setValue(SK_SATURATION, v); }

qreal
Settings::saturation() const
{ return value(SK_SATURATION, 1.0).toReal(); }

void
Settings::setBrightness(qreal v)
{ setValue(SK_BRIGHTNESS, v); }

qreal
Settings::brightness() const
{ return value(SK_BRIGHTNESS, 1.0).toReal(); }

// - Transform -

void
Settings::setAnnotationScale(qreal v)
{
  if (qFuzzyCompare(v, 1))
    remove(SK_ANNOTSCALE);
  else
    setValue(SK_ANNOTSCALE, v);
}

qreal
Settings::annotationScale() const
{ return value(SK_ANNOTSCALE, 1.0).toReal(); }

// EOF

/*
void
Settings::setAnnotationRotation(qreal v)
{ setValue(SK_ANNOTROTATE, v); }

qreal
Settings::annotationRotation() const
{ return value(SK_ANNOTROTATE).toReal(); }
*/
