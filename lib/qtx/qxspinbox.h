#ifndef QXSPINBOX_H
#define QXSPINBOX_H

// qxspinbox.h
// 5/30/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QSpinBox>

QTX_BEGIN_NAMESPACE

class QxSpinBox : public QSpinBox
{
  Q_OBJECT
  Q_DISABLE_COPY(QxSpinBox)
  typedef QxSpinBox Self;
  typedef QSpinBox Base;

public:
  explicit QxSpinBox(QWidget *parent = nullptr)
    : Base(parent) { }

  QLineEdit *lineEdit() const { return Base::lineEdit(); }

signals:
  void returnPressed();

  // - Events -
protected:
  void keyPressEvent(QKeyEvent *event) override;
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

QTX_END_NAMESPACE

#endif // QXSPINBOX_H
