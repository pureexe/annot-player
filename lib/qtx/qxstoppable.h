#ifndef QXSTOPPABLE_H
#define QXSTOPPABLE_H

// qxstoppable.h
// 2/14/2011

#include "qtx/qtxglobal.h"
#include <QtCore/QRunnable>
#include <QtCore/QThread>

QTX_BEGIN_NAMESPACE

class QxStoppable
{
public:
  virtual void stop() = 0;
};

class QxStoppableTask : public QRunnable, public QxStoppable
{ };

class QxStoppableThread : public QThread, public QxStoppable
{
  Q_OBJECT
  Q_DISABLE_COPY(QxStoppableThread)
  typedef QThread Base;
public:
  explicit QxStoppableThread(QObject *parent = nullptr) : Base(parent) { }
public slots:
  void run() override { Base::run(); }
  void stop() override { }
};

QTX_END_NAMESPACE

#endif // STOPPABLE_H
