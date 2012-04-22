#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

// downloadtask.h
// 2/20/2012

#include "module/qtext/stoppable.h"
#include <QtCore/QObject>
#include <QtCore/QThreadPool>
#include <QtCore/QString>
#include <QtCore/QList>

struct DownloadTaskInfo
{
  int state;
  QString url;
  QString title;
  QString fileName;

public:
  enum Field { // used for serialization
    StateField = 0, UrlField, TitleField, FileNameField,
    FieldCount // 4
  };

  DownloadTaskInfo() : state(0) { }
  DownloadTaskInfo(int state_, const QString &url_, const QString &title_, const QString &fileName_)
    : state(state_), url(url_), title(title_), fileName(fileName_) { }

  void clear() { state = 0; url.clear(); title.clear(); fileName.clear(); }
};
typedef QList<DownloadTaskInfo> DownloadTaskInfoList;

class DownloadTask : public QObject, public StoppableTask
{
  Q_OBJECT
  typedef DownloadTask Self;
  typedef QObject Base;

public:
  enum State { Error = -1, Stopped = 0, Downloading = 1, Pending, Finished };

  explicit DownloadTask(const QString &url, QObject *parent = 0);
  explicit DownloadTask(const DownloadTaskInfo &info, QObject *parent = 0);
private:
  void init();

signals:
  void titleChanged(QString title);
  void fileNameChanged(QString fileName);
  void stateChanged(int state);
  void progress(qint64 receivedBytes, qint64 totalBytes);
  void finished(DownloadTask *self);
  void error(QString message);

  // - Properties -
public:
  DownloadTaskInfo info() const
  { return DownloadTaskInfo(state_, url_, title_, fileName_); }

  int id() const { return id_; }
  State state() const { return state_; }
  const QString &title() const { return title_; }
  const QString &fileName() const { return fileName_; }
  const QString &downloadPath() const { return downloadPath_; }
  const QString &url() const { return url_; }

  bool isStopped() const { return state_ == Stopped; }
  bool isPending() const { return state_ == Pending; }
  bool isRunning() const { return state_ == Downloading; }
  bool isFinished() const { return state_ == Finished; }

  qint64 size() const { return totalSize_; }
  qint64 downloadSize() const { return receivedSize_; }
  qreal speed() const { return speed_; } ///< bps
  qint64 remainingTime() const { return speed_ ? (totalSize_ - receivedSize_) / speed_ * 1000 : 0.0; } ///< in msec
  qreal percentage() const { return totalSize_ ? qreal(receivedSize_)/totalSize_ : 0.0; }

  // - Actions -
public slots:
  virtual void run() = 0; ///< \override
  virtual void stop() = 0; ///< \override

  //virtual void finish()
  //{ emit progress(totalSize_, totalSize_); emit finished(this); }
  void setState(State state);
  virtual void reset() { setState(Stopped); }
  void setDownloadPath(const QString &path) { downloadPath_ = path; }

  void start() { reset(); QThreadPool::globalInstance()->start(this); }
  void startLater(qint64 msecs);

protected slots:
  void setTitle(const QString &title) { title_ = title; emit titleChanged(title_); }
  void setFileName(const QString &fileName) { fileName_ = fileName; emit fileNameChanged(fileName_); }

  void updateProgress(qint64 receivedBytes, qint64 totalBytes);

  // - Implementation -
private:
  static int count_;

  int id_;
  qint64 receivedSize_, totalSize_;
  qint64 progressUpdateTime_; // in msec
  qreal speed_;
  QString downloadPath_;

  // Download task info
  State state_;
  QString url_;
  QString title_;
  QString fileName_;
};

class DownloadTaskList : public QList<DownloadTask *>
{
  typedef DownloadTaskList Self;
  typedef QList<DownloadTask *> Base;
public:
  DownloadTaskList() { }
  DownloadTaskList(const Self &that) : Base(that) { }

  DownloadTaskInfoList infoList() const
  {
    DownloadTaskInfoList ret;
    foreach (value_type v, *this)
      if (v)
        ret.append(v->info());
    return ret;
  }
};

#endif // DOWNLOADTASK_H
