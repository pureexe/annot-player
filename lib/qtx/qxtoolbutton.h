#ifndef QXTOOLBUTTON_H
#define QXTOOLBUTTON_H

// qxtoolbutton.h
// 8/6/2011

#include "qtx/qtxglobal.h"
#include <QtGui/QToolButton>

QTX_BEGIN_NAMESPACE

class QxToolButton : public QToolButton
{
  Q_OBJECT
  Q_DISABLE_COPY(QxToolButton)
  typedef QxToolButton Self;
  typedef QToolButton Base;

public:
  explicit QxToolButton(QWidget *parent = nullptr)
    : Base(parent) { }

  // - Events -
protected:
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

QTX_END_NAMESPACE

#endif // QXTOOLBUTTON_H
