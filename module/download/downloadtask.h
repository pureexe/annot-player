#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

// downloadtask.h
// 2/20/2012

#include "module/qtext/stoppable.h"
#include <QObject>
#include <QString>
#include <QList>

class DownloadTask : public QObject, public StoppableTask
{
  Q_OBJECT
  typedef DownloadTask Self;
  typedef QObject Base;

public:
  enum State { Error = -1, Stopped = 0, Downloading = 1, Pending, Finished };

  explicit DownloadTask(const QString &url, QObject *parent = 0);

signals:
  void titleChanged(QString title);
  void fileNameChanged(QString fileName);
  void stateChanged(int state);
  void progress(qint64 receivedBytes, qint64 totalBytes);
  void finished(DownloadTask *self);
  void error(QString message);

  // - Properties -
public:
  int id() const { return id_; }
  State state() const { return state_; }
  const QString &title() const { return title_; }
  const QString &fileName() const { return fileName_; }
  const QString &url() const { return url_; }

  bool isStopped() const { return state_ == Stopped; }
  bool isRunning() const { return state_ == Downloading || state_ == Pending; }
  bool isFinished() const { return state_ == Finished; }

  qint64 size() const { return totalSize_; }
  qint64 downloadSize() const { return receivedSize_; }
  qreal speed() const { return speed_; } ///< bps
  qint64 remainingTime() const { return speed_ ? (totalSize_ - receivedSize_) / speed_ * 1000 : 0.0; } ///< in msec
  qreal percentage() const { return totalSize_ ? (qreal)receivedSize_ / totalSize_ : 0.0; }

  // - Actions -
public slots:
  virtual void run() = 0; ///< \override
  virtual void stop() = 0; ///< \override

  //virtual void finish()
  //{ emit progress(totalSize_, totalSize_); emit finished(this); }

protected slots:
  void setState(State state);
  void setTitle(const QString &title) { title_ = title; emit titleChanged(title_); }
  void setFileName(const QString &fileName) { fileName_ = fileName; emit fileNameChanged(fileName_); }

  void updateProgress(qint64 receivedBytes, qint64 totalBytes);

  // - Implementation -
private:
  static int count_;

  int id_;
  State state_;
  QString url_;
  QString title_;
  QString fileName_;

  qint64 receivedSize_, totalSize_;
  qint64 progressUpdateTime_; // in msec
  qreal speed_;
};

typedef QList<DownloadTask*> DownloadTaskList;

#endif // DOWNLOADTASK_H
