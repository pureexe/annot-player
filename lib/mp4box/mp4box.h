#ifndef MP4BOX_H
#define MP4BOX_H

// mp4box.h
// 2/15/2012

#include <QtCore/QString>
#include <QtCore/QStringList>

namespace Mp4Box {

  struct Mp4Info
  {
    int width,  height;
    Mp4Info() : width(0), height(0) { }
  };

  bool muxMp4File(const QString &mp4, const QStringList &tracks,
                  Mp4Info *info = nullptr);

} // namepsace MP4Box

#endif // MP4BOX_H
