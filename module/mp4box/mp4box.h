#ifndef MP4BOX_H
#define MP4BOX_H

// mp4box.h
// 2/15/2012

#include <QString>
#include <QStringList>

namespace Mp4Box {

  bool muxMp4File(const QString &mp4, const QStringList &tracks);

} // namepsace MP4Box

#endif // MP4BOX_H
