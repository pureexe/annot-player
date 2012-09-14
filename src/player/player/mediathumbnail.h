#ifndef MEDIATHUMBNAIL_H
#define MEDIATHUMBNAIL_H

// mediathumbnail.h
// 6/7/2012

#include <QtGui/QLabel>

class MediaThumbnail : public QLabel
{
  Q_OBJECT
  Q_DISABLE_COPY(MediaThumbnail)
  typedef MediaThumbnail Self;
  typedef QLabel Base;

  QString mediaId_;
  qint64 time_;

public:
  explicit MediaThumbnail(QWidget *parent = nullptr)
    : Base(parent) { }

public:
  qint64 time() const { return time_; }
  void setTime(qint64 msecs) { time_ = msecs; }

  void setMediaId(const QString &digest) { mediaId_ = digest; }
  const QString &mediaId() const { return mediaId_; }

  // CHECKPOINT
  // use ffmpeg to provide thumbnails for image
  // http://stackoverflow.com/questions/1908411/how-to-use-ffmpeg
  // http://stackoverflow.com/questions/8679390/ffmpeg-extracting-20-images-from-a-video-of-variable-length

//public slots:
//  void setVisible(bool visible) override;
};

#endif // MEDIATHUMBNAIL_H
