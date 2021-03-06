#ifndef MOUSEHOLDEVENTLISTENER_H
#define MOUSEHOLDEVENTLISTENER_H

// mouseholdeventlistener.h
// 7/14/2012

#include "lib/eventlistener/eventlistener.h"
#include <QtCore/QPoint>

QT_FORWARD_DECLARE_CLASS(QTimer)

class MouseHoldEventListener : public EventListener
{
  Q_OBJECT
  Q_DISABLE_COPY(MouseHoldEventListener)
  typedef MouseHoldEventListener Self;
  typedef EventListener Base;

  QPoint pressPos_, movePos_;
  QTimer *timer_;

public:
  explicit MouseHoldEventListener(QObject *parent = nullptr);

signals:
  void triggered();

public:
  qint64 interval() const;
public slots:
  void setInterval(qint64 msecs);
  void clear(); ///< clear status

protected:
  void eventListener(QEvent *event) override;
};

#endif // MOUSEHOLDEVENTLISTENER_H
