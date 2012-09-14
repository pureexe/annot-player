#ifndef STREAMSERVICE_H
#define STREAMSERVICE_H

// streamservice.h
// 2/11/2012

#include <QtCore/QObject>
#include <climits>
#include "lib/mediacodec/codecinfo.h"

class StreamThread;
class InputStream;
class MediaToc;

class StreamService : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(StreamService)
  typedef StreamService Self;
  typedef QObject Base;

  StreamThread *t_;

  // - Construction -
public:
  static Self *globalInstance() { static Self g; return &g; }
protected:
  explicit StreamService(QObject *parent = nullptr);
  ~StreamService();

signals:
  void streamReady(QString url);
  void message(QString msg);
  void errorMessage(QString msg);

  // - Service -
public:
  bool isActive() const;
public slots:
  void start();
  void stop(ulong timeout = ULONG_MAX);
  void terminateService();

  void restart(ulong timeout = ULONG_MAX)
  { if (isActive()) stop(timeout); start(); }

  // - Properties -
public:
  bool hasStream() const;
  int addStream(MediaType t, InputStream *in, MediaToc *toc = nullptr);
public slots:
  void setDuration(qint64 msecs);
  void removeStream(int streamId);
  void clear(); ///< remove streams, and reset durations

  QString url() const; ///< RTSP streaming URL
};

#endif // STREAMSERVICE_H
