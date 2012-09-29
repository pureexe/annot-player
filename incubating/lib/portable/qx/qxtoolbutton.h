#ifndef QXTOOLBUTTON_H
#define QXTOOLBUTTON_H

// qxtoolbutton.h
// 8/6/2011

#include "qx/qxglobal.h"
#include <QtWidgets/QToolButton>

QX_BEGIN_NAMESPACE

class QxToolButton : public QToolButton
{
  Q_OBJECT
  Q_DISABLE_COPY(QxToolButton)
  QX_EXTEND_CLASS(QxToolButton, QToolButton)

public:
  explicit QxToolButton(QWidget *parent = nullptr)
    : Base(parent) { }

  // - Events -
protected:
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

QX_END_NAMESPACE

#endif // QTOOLBUTTON_H
