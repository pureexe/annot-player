#ifndef _VLCCORE_HTTPSTREAMSESSION_H
#define _VLCCORE_HTTPSTREAMSESSION_H

// vlccore/httpstreamsession.h
// 3/14/2012
#ifdef _MSC_VER
  #pragma warning (disable:4819)       // C4819: The file contains a character that cannot be represented in the current code page.
  #pragma warning (disable:4996)       // C4996: MS' deprecated std functions orz.
#endif // _MSC_VER

#include "module/vlccore/httpsession.h"
#include "module/qtext/stoppable.h"
#include "module/stream/inputstream.h"
#include "module/stream/fifostream.h"
#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QUrl>
#include <QList>
#include <QHash>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)
QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

class RemoteStream;

class HttpStreamSession : public VlcHttpSession
{
  Q_OBJECT
  typedef HttpStreamSession Self;
  typedef VlcHttpSession Base;

  QList<QUrl> urls_;
  qint64 duration_;
  InputStreamList ins_;
  FifoStream *fifo_;
  StoppableTask *merger_;

  QMutex m_;
  QWaitCondition readyCond_, readyReadCond_, stoppedCond_;
  bool ready_;

  QString fileName_;

  StoppableTask *progressTask_;

  //struct Progress
  //{
  //  qint64 receivedBytes, totalBytes;
  //  explicit Progress(qint64 r = 0, qint64 t = 0) : receivedBytes(r), totalBytes(t) { }
  //};
  //QHash<long, Progress> progress_;

public:
  explicit HttpStreamSession(QObject *parent = 0)
    : Base(parent), duration_(0), fifo_(0), merger_(0),
      ready_(false), progressTask_(0) { }

  HttpStreamSession(const QList<QUrl> &urls, qint64 duration, QObject *parent = 0)
    : Base(parent), urls_(urls), duration_(duration), fifo_(0), merger_(0),
      ready_(false), progressTask_(0) { }

  ~HttpStreamSession();

signals:
  void error(QString msg);
  void message(QString msg);
  void fileSaved(QString msg);
  void progress(qint64 receivedBytes, qint64 totalBytes);

  // - Properties -
public:
  const QList<QUrl> &urls() const { return urls_; }
  virtual QString contentType() const; ///< \override

  virtual qint64 read(char *data, qint64 maxSize)  ///< \override
  { return fifo_ ? fifo_->read(data, maxSize) : 0; }

  virtual bool seek(qint64 pos) ///< \override
  { return fifo_ ? fifo_->seek(pos) : false; } ///< \override

  virtual qint64 size() const { return fifo_ ? fifo_->size() : 0; } ///< \override
  virtual qint64 pos() const { return fifo_ ? fifo_->pos() : 0; } ///< \override
  virtual qint64 availableSize() const { return fifo_ ? fifo_->availableSize() : 0; } ///< \override

  qint64 receivedSize() const;

public slots:
  void setUrls(const QList<QUrl> &urls) { urls_ = urls; }
  void setDuration(qint64 duration) { duration_ = duration; }
  virtual void run(); ///< \override
  virtual void stop(); ///< \override

  virtual void waitForReady(); ///< \override
  virtual void waitForStopped(); ///< \override

  void updateProgress() { emit progress(receivedSize(), size()); }

protected slots:
  void invalidateSize();
  void invalidateFileName();
  void finish();
  void save();

  void wakeAllStopped() { stoppedCond_.wakeAll(); }
  void wakeAllReady() { readyCond_.wakeAll(); }
  void wakeAllReadyRead() { readyReadCond_.wakeAll(); }

  //void updateProgressWithId(qint64 receivedBytes, qint64 totalBytes, long id);
};

#endif // _VLCCORE_HTTPSTREAMSESSION_H
