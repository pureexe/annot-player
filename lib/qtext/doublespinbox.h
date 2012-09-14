#ifndef _QTEXT_DOUBLESPINBOX_H
#define _QTEXT_DOUBLESPINBOX_H

// doublespinbox.h
// 8/21/2012

#include <QtGui/QDoubleSpinBox>

namespace QtExt {

class DoubleSpinBox : public QDoubleSpinBox
{
  Q_OBJECT
  Q_DISABLE_COPY(DoubleSpinBox)
  typedef DoubleSpinBox Self;
  typedef QDoubleSpinBox Base;

public:
  explicit DoubleSpinBox(QWidget *parent = nullptr)
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

} // namespace QtExt

#endif // _QTEXT_DOUBLESPINBOX_H
