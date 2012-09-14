#ifndef MOUSECLICKEVENTLISTENER_H
#define MOUSECLICKEVENTLISTENER_H

// mouseclickeventlistener.h
// 7/14/2012
// Detect single click

#include "lib/eventlistener/eventlistener.h"
#include <QtCore/QPoint>

QT_FORWARD_DECLARE_CLASS(QTimer)

class MouseClickEventListener : public EventListener
{
  Q_OBJECT
  Q_DISABLE_COPY(MouseClickEventListener)
  typedef MouseClickEventListener Self;
  typedef EventListener Base;

  QPoint pressPos_, movePos_;
  ulong pressButtons_;
  bool pressed_;
  qint64 pressTime_;

  QTimer *timer_;

public:
  explicit MouseClickEventListener(QObject *parent = nullptr);

signals:
  void triggered();

public:
  qint64 interval() const;
public slots:
  void setInterval(qint64 msecs);
  void clear();

protected:
  void eventListener(QEvent *event) override;
protected slots:
  void updateEnabled(bool t);
  void start();
  void stop();
};

#endif // MOUSECLICKEVENTLISTENER_H
