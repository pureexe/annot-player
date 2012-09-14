#ifndef STREAMTHREAD_H
#define STREAMTHREAD_H

// streamer.h
// 2/11/2012

#include <QtCore/QThread>
#include <QtCore/QString>
#include <QtCore/QList>
#include "lib/mediacodec/codecinfo.h"

class InputStream;
class MediaToc;

struct MediaStreamInfo
{
  MediaType   mediaType;
  InputStream *inputStream;
  MediaToc    *mediaToc;

  explicit MediaStreamInfo(MediaType t, InputStream *s = nullptr, MediaToc *toc = nullptr)
    : mediaType(t), inputStream(s), mediaToc(toc) { }
};

class StreamThread : public QThread
{
  Q_OBJECT
  Q_DISABLE_COPY(StreamThread)
  typedef StreamThread Self;
  typedef QThread Base;

  QList<MediaStreamInfo> ins_;
  qint64 duration_;

  char stop_;

  int port_;
  QString url_;

public:
  explicit StreamThread(QObject *parent = nullptr)
    : Base(parent), duration_(0), port_(0)
  { initLive555(); }

private:
  static void initLive555();

signals:
  void streamReady(QString url);
  void message(QString msg);
  void errorMessage(QString msg);

public:
  int port() const { return port_; }
  const QString &url() const { return url_; } ///< RTSP streaming URL

  bool hasStream() const { return !ins_.isEmpty(); }

  int addStream(MediaType t, InputStream *in, MediaToc *toc)
  { ins_.append(MediaStreamInfo(t, in, toc)); return ins_.size() - 1; }

public slots:
  void setPort(int port) { port_ = port; }
  void stop(ulong timeout = ULONG_MAX);

  void removeStream(int index)
  { if (index >= 0 && index < ins_.size()) ins_.removeAt(index); }

  void removeStreams() { ins_.clear(); }
  void setDuration(qint64 msecs) { duration_ = msecs; }

protected slots:
  void run() override;

  void onStreamReady()
  { if (!url_.isEmpty()) emit streamReady(url_); }

  void shufflePort();
};

#endif // STREAMTHREAD_H
