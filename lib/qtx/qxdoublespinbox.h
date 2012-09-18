#ifndef QXDOUBLESPINBOX_H
#define QXDOUBLESPINBOX_H

// qxdoublespinbox.h
// 8/21/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QDoubleSpinBox>

QTX_BEGIN_NAMESPACE

class QxDoubleSpinBox : public QDoubleSpinBox
{
  Q_OBJECT
  Q_DISABLE_COPY(QxDoubleSpinBox)
  typedef QxDoubleSpinBox Self;
  typedef QDoubleSpinBox Base;

public:
  explicit QxDoubleSpinBox(QWidget *parent = nullptr)
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

#endif // QXDOUBLESPINBOX_H
