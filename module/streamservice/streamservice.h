#ifndef STREAMSERVICE_H
#define STREAMSERVICE_H

// streamservice.h
// 2/11/2012

#include <QObject>
#include <climits>
#include "module/mediacodec/codecinfo.h"

class StreamThread;
class InputStream;
class MediaToc;

class StreamService : public QObject
{
  Q_OBJECT
  typedef StreamService Self;
  typedef QObject Base;

  StreamThread *t_;

  // - Construction -
public:
  static Self *globalInstance() { static Self g; return &g; }
protected:
  explicit StreamService(QObject *parent = 0);
  ~StreamService();

signals:
  void streamReady(QString url);
  void errorReceived(QString msg);
  void messageReceived(QString msg);

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
  int addStream(MediaType t, InputStream *in, MediaToc *toc = 0);
public slots:
  void setDuration(qint64 msecs);
  void removeStream(int streamId);
  void clear(); ///< remove streams, and reset durations

  QString url() const; ///< RTSP streaming URL
};

#endif // STREAMSERVICE_H
