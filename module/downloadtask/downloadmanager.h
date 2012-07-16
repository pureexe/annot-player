#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

// downloadmanager.h
// 2/20/2012

#include "module/downloadtask/downloadtask.h"
#include <QtCore/QObject>

class AnnotationDownloader;
class DownloadManager : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(DownloadManager)
  typedef DownloadManager Self;
  typedef QObject Base;

  DownloadTaskList tasks_;

  int threadCount_;
#ifdef WITH_MODULE_ANNOTDOWN
  AnnotationDownloader *downloader_;
#else
  QString downloadsLocation_;
#endif // WITH_MODULE_ANNOTDOWN

public:
  explicit DownloadManager(QObject *parent = 0);

signals:
  void taskAdded(DownloadTask *t);
  void taskRemoved(DownloadTask *t);

  void message(QString msg);
  void error(QString msg);
  void warning(QString msg);

  // - Properties -
public:
  const DownloadTaskList &tasks() const { return tasks_; }

  bool isEmpty() const { return tasks_.isEmpty(); }
  bool isRunning() const;
  bool isIdle() const { return !isRunning(); }

  void addTask(DownloadTask *t);

  DownloadTask *taskWithId(int tid);
  DownloadTask *taskWithUrl(const QString &url);
  void removeTask(DownloadTask *t); // also delete task if owned

  int maxThreadCount() const { return threadCount_; }

  QString downloadsLocation() const;
public slots:
  void setDownloadsLocation(const QString &path);
  void downloadAnnotation(const QString &refurl);
protected slots:
  void downloadAnnotation(const QString &url, const QString &refurl, const QString &title);

public slots:
  void stopAll();
  void removeAll();
  void refreshSchedule();
  void setMaxThreadCount(int n);

protected:
  QString normalizeUrl(const QString &url);
};

#endif // DOWNLOADMANAGER_H
