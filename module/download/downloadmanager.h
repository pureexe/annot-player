#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

// downloadmanager.h
// 2/20/2012

#include "downloadtask.h"
#include <QObject>
#include <QList>

class DownloadManager : public QObject
{
  Q_OBJECT
  typedef DownloadManager Self;
  typedef QObject Base;

  DownloadTaskList tasks_;

public:
  explicit DownloadManager(QObject *parent = 0)
    : Base(parent) { }

  // - Properties -
public:
  const DownloadTaskList &tasks() const { return tasks_; }

  bool isEmpty() const { return tasks_.isEmpty(); }

  void addTask(DownloadTask *t) ///< t will be automatically deleted
  { Q_ASSERT(t); tasks_.append(t); }

  DownloadTask *taskWithId(int tid);
  void removeTaskWithId(int tid);

public slots:
  void stopAll();
};

#endif // DOWNLOADMANAGER_H
