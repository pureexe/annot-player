#ifndef MEDIASTREAMER_H
#define MEDIASTREAMER_H

// mediastreamer.h
// 2/16/2016

#include <QObject>
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

class MediaStreamTask;

struct MediaStreamInfo
{
  QStringList urls;
  QList<qint64> durations;
  QList<qint64> sizes;
};

class MediaStreamer : public QObject
{
  Q_OBJECT
  typedef MediaStreamer Self;
  typedef QObject Base;

  MediaStreamTask *t_;

  // - Construction -
public:
  static Self *globalInstance() { static Self g; return &g; }

protected:
  explicit MediaStreamer(QObject *parent = 0)
    : Base(parent), t_(0) { }
  ~MediaStreamer() { stop(); }

  // - Actions -
public:
  ///  Network access manager will be deleted after used.
  void stream(const MediaStreamInfo &msi, QNetworkAccessManager *nam = 0, bool async = true);

public slots:
  void stop();
};

#endif // MEDIASTREAMER_H
