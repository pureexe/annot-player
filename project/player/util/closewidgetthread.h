#ifndef CLOSEWIDGETTHREAD_H
#define CLOSEWIDGETTHREAD_H

// CloseWidgetThread.h
// 10/31/2011

#include <QThread>
#include <QWidget>

class CloseWidgetThread : public QThread
{
  Q_OBJECT

  QWidget *w_;

  virtual void run() { w_->close(); } // \override

public:
  explicit CloseWidgetThread(QWidget *w, QObject *parent = 0)
    : QThread(parent), w_(w) { Q_ASSERT(w_); }
};


#endif // CLOSEWIDGETTHREAD_H
