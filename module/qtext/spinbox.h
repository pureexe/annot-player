#ifndef _QTEXT_SPINBOX_H
#define _QTEXT_SPINBOX_H

// qtext/spinbox.h
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
  explicit SpinBox(QWidget *parent = 0)
    : Base(parent) { }

  QLineEdit *lineEdit() const { return Base::lineEdit(); }

signals:
  void returnPressed();

  // - Events -
protected:
  virtual void keyPressEvent(QKeyEvent *event);
  virtual void enterEvent(QEvent *event); ///< \reimp
  virtual void leaveEvent(QEvent *event); ///< \reimp
};

} // namespace QtExt

#endif // _QTEXT_SPINBOX_H
