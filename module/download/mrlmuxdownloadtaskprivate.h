#ifndef MRLMUXDOWNLOADTASKPRIVATE_H
#define MRLMUXDOWNLOADTASKPRIVATE_H

// mrlmuxdownloadtaskprivate.h
// 2/23/2012

#include <QObject>

class ProgressWithId : public QObject
{
  Q_OBJECT
  typedef QObject Base;

  long id_;

public:
  ProgressWithId(long id, QObject *parent = 0)
    : Base(parent), id_(id) { }

signals:
  void progress(qint64 receivedBytes, qint64 totalBytes, long id);

public slots:
  void emit_progress(qint64 receivedBytes, qint64 totalBytes)
  { emit progress(receivedBytes, totalBytes, id_); }
};

#endif // MRLMUXDOWNLOADTASKPRIVATE_H
