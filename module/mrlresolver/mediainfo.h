#ifndef MEDIAINFO_H
#define MEDIAINFO_H

// mediainfo.h
// 1/25/2011

#include <QMetaType>
#include <QList>
#include <QStringList>

struct MediaInfo
{
  QString title;
  QString refurl;
  QString suburl;
  QStringList mrls;
};
Q_DECLARE_METATYPE(MediaInfo)

#endif // MEDIAINFO_H
