#ifndef QXCHECKBOX_H
#define QXCHECKBOX_H

// qxcheckbox.h
// 5/30/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QCheckBox>

QTX_BEGIN_NAMESPACE

class QxCheckBox : public QCheckBox
{
  Q_OBJECT
  Q_DISABLE_COPY(QxCheckBox)
  typedef QxCheckBox Self;
  typedef QCheckBox Base;

public:
  explicit QxCheckBox(QWidget *parent = nullptr)
    : Base(parent) { }

  explicit QxCheckBox(const QString &text, QWidget *parent = nullptr)
    : Base(text, parent) { }

  // - Events -
protected:
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

QTX_END_NAMESPACE

#endif // QXCHECKBOX_H
