#ifndef MP4BOX_H
#define MP4BOX_H

// mp4box.h
// 2/15/2012

#include <QString>
#include <QStringList>

namespace MP4Box {

  bool muxFile(const QString &mp4, const QStringList &tracks);

} // namepsace MP4Box

#endif // MP4BOX_H
