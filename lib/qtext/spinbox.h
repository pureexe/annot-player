#ifndef _QTEXT_SPINBOX_H
#define _QTEXT_SPINBOX_H

// spinbox.h
// 5/30/2012

#include <QtGui/QSpinBox>

namespace QtExt {

class SpinBox : public QSpinBox
{
  Q_OBJECT
  Q_DISABLE_COPY(SpinBox)
  typedef SpinBox Self;
  typedef QSpinBox Base;

public:
  explicit SpinBox(QWidget *parent = nullptr)
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

#endif // _QTEXT_SPINBOX_H
