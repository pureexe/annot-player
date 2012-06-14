#ifndef _QTEXT_RADIOBUTTON_H
#define _QTEXT_RADIOBUTTON_H

// qtext/radiobutton.h
// 5/30/2012

#include <QtGui/QRadioButton>

namespace QtExt {

class RadioButton : public QRadioButton
{
  Q_OBJECT
  Q_DISABLE_COPY(RadioButton)
  typedef RadioButton Self;
  typedef QRadioButton Base;

public:
  explicit RadioButton(QWidget *parent = 0)
    : Base(parent) { }

  explicit RadioButton(const QString &text, QWidget *parent = 0)
    : Base(text, parent) { }

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \reimp
  virtual void leaveEvent(QEvent *event); ///< \reimp
};

} // namespace QtExt

#endif // _QTEXT_RADIOBUTTON_H
