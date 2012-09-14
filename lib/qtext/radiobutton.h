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
  explicit RadioButton(QWidget *parent = nullptr)
    : Base(parent) { }

  explicit RadioButton(const QString &text, QWidget *parent = nullptr)
    : Base(text, parent) { }

  // - Events -
protected:
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

} // namespace QtExt

#endif // _QTEXT_RADIOBUTTON_H
