#ifndef QXRADIOBUTTON_H
#define QXRADIOBUTTON_H

// qxradiobutton.h
// 5/30/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QRadioButton>

QTX_BEGIN_NAMESPACE

class QxRadioButton : public QRadioButton
{
  Q_OBJECT
  Q_DISABLE_COPY(QxRadioButton)
  typedef QxRadioButton Self;
  typedef QRadioButton Base;

public:
  explicit QxRadioButton(QWidget *parent = nullptr)
    : Base(parent) { }

  explicit QxRadioButton(const QString &text, QWidget *parent = nullptr)
    : Base(text, parent) { }

  // - Events -
protected:
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

QTX_END_NAMESPACE

#endif // QXRADIOBUTTON_H
