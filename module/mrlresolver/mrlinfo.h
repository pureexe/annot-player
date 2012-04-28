#ifndef MRLINFO_H
#define MRLINFO_H

// mrlinfo.h
// 1/25/2011

#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QString>

struct MrlInfo
{
  QString url;
  qint64 duration; ///< in msec
  qint64 size;

  explicit MrlInfo(const QString url_ = QString(), qint64 duration = 0, qint64 size = 0)
    : url(url_), duration(duration), size(size) { }
};
typedef QList<MrlInfo> MrlInfoList;

struct MediaInfo
{
  QString title,
          refurl,
          suburl;
  MrlInfoList mrls;
};

Q_DECLARE_METATYPE(MrlInfo)
Q_DECLARE_METATYPE(MediaInfo)

#endif // MRLINFO_H
