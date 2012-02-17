#ifndef MP4CODEC_H
#define MP4CODEC_H

// mp4codec.h
// 2/12/2012

#include <QObject>
#include <QStringList>

class InputStream;

class MP4Codec : public QObject
{
  Q_OBJECT
  typedef MP4Codec Self;
  typedef QObject Base;

  // - Construction -
public:
  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit MP4Codec(QObject *parent = 0) : Base(parent) { }

public:
  bool muxStream(const QString &path, qint64 duration, InputStream *vin, InputStream *ain);

  static bool isMP4Stream(InputStream *in);
};

#endif // MP4CODEC_H
