#ifndef ACICONBUTTON_H
#define ACICONBUTTON_H

// aciconbutton.h
// 5/16/2012

#include "qtx/qxtoolbutton.h"

QT_FORWARD_DECLARE_CLASS(QPropertyAnimation)
QT_FORWARD_DECLARE_CLASS(QTimer)

typedef QxToolButton AcIconButtonBase;
class AcIconButton : public AcIconButtonBase
{
  Q_OBJECT
  Q_PROPERTY(int radius READ radius WRITE setRadius)
  Q_PROPERTY(int minimumRadius READ minimumRadius WRITE setMinimumRadius)
  Q_PROPERTY(int maximumRadius READ maximumRadius WRITE setMaximumRadius)
  Q_DISABLE_COPY(AcIconButton)
  typedef AcIconButton Self;
  typedef AcIconButtonBase Base;

  int radius_, minimumRadius_, maximumRadius_;
  QPropertyAnimation *radiusAni_;

  QTimer *leaveTimer_;

public:
  explicit AcIconButton(QWidget *parent = nullptr);

  // - Properties -
public:
  int radius() const { return radius_; }
  int minimumRadius() const { return minimumRadius_; }
  int maximumRadius() const { return maximumRadius_; }
public slots:
  void setRadius(int value);
  void setMinimumRadius(int value) { minimumRadius_ = value; }
  void setMaximumRadius(int value) { maximumRadius_ = value; }

  // - Events -
public slots:
  void enter();
  void leave();
protected:
  void enterEvent(QEvent *event);
  void leaveEvent(QEvent *event);
  void changeEvent(QEvent *event);
};

#endif // ACICONBUTTON_H
