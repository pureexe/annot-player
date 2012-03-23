#ifndef MRLINFO_H
#define MRLINFO_H

// mrlinfo.h
// 1/25/2011

#include <QMetaType>
#include <QString>
#include <QList>

struct MrlInfo
{
  QString url;
  qint64 duration; ///< in msec
  qint64 size;

  explicit MrlInfo(const QString _url = QString(), qint64 duration = 0, qint64 size = 0)
    : url(_url), duration(duration), size(size) { }
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
