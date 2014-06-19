// annotationcachemanager.cc
// 6/27/2012
#include "lib/annotcache/annotationcachemanager.h"
#include <QtCore>

//#define DEBUG "annotationcachemanager"
#include "qtx/qxdebug.h"

// - Header -

QString
AnnotationCacheManager::xmlHeader(const QString &url, const QDateTime &ts)
{
  static const QString fmt =
    "<!-- annotation\n"
    "\\version 0.1\n"
    "\\time %1\n"
    "\\url %2\n"
    "-->\n";
  return fmt
      .arg(ts.toString(Qt::ISODate))
      .arg(url);
}

QString
AnnotationCacheManager::jsonHeader(const QString &url, const QDateTime &ts)
{
  static const QString fmt =
    "/** annotation\n"
    " *  \\version 0.1\n"
    " *  \\time %1\n"
    " *  \\url %2\n"
    " */\n";
  return fmt
      .arg(ts.toString(Qt::ISODate))
      .arg(url);
}

// - Save/Load -

QString
AnnotationCacheManager::hashFileName(const QString &url)
{
  QRegExp rx("nicovideo.jp/watch/(.*)", Qt::CaseInsensitive);
  if (rx.indexIn(url) >= 0)
    return rx.cap(1)
      .remove(QRegExp("/.*"))
      .append(".xml")
      .prepend("nico_");

  rx = QRegExp("bilibili.com/video/(.*)", Qt::CaseInsensitive);
  if (rx.indexIn(url) >= 0) {
    QString ret = rx.cap(1);
    rx = QRegExp("index_(\\d).html", Qt::CaseInsensitive);
    if (rx.indexIn(url) < 0)
      ret.remove(QRegExp("/.*"));
    else {
      QString part = rx.cap(1);
      ret.remove(QRegExp("/.*"));
      ret.append('_').append(part);
    }
    return ret
      .append(".xml")
      .prepend("bili_");
  }

  rx = QRegExp("acfun.tv/v/(.*)", Qt::CaseInsensitive);
  if (rx.indexIn(url) >= 0) {
    QString ret = rx.cap(1);
    rx = QRegExp("index_(\\d).html", Qt::CaseInsensitive);
    if (rx.indexIn(url) < 0)
      ret.remove(QRegExp("/.*"));
    else {
      QString part = rx.cap(1);
      ret.remove(QRegExp("/.*"));
      ret.append('_').append(part);
    }
    return ret
      .append(".json")
      .prepend("acfun_");

  }

  return QString();
}

QString
AnnotationCacheManager::cacheFile(const QString &url) const
{
  QString ret = hashFileName(url);
  return ret.isEmpty() ? ret :
         ret.prepend('/').prepend(location_);
}

bool
AnnotationCacheManager::saveData(const QByteArray &data, const QString &url)
{
  QString fileName = cacheFile(url);
  if (fileName.isEmpty())
    return false;
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly))
    return false;

  QString header =  fileName.endsWith(".xml") ? xmlHeader(url, QDateTime::currentDateTime())
                                              : jsonHeader(url, QDateTime::currentDateTime());
  file.write(header.toLocal8Bit());
  file.write(data);
  return file.error() == QFile::NoError;
}

bool
AnnotationCacheManager::saveFile(const QString &fileName, const QString &url)
{
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return false;
  return saveData(file.readAll(), url);
}

QString
AnnotationCacheManager::findFile(const QString &url)
{
  QString fileName = cacheFile(url);
  return QFile::exists(fileName) ? fileName : QString();
}

// EOF
