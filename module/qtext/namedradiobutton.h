#ifndef _QTEXT_NAMEDRADIOBUTTON_H
#define _QTEXT_NAMEDRADIOBUTTON_H

// qtext/namedradiobutton.h
// 5/30/2012

#include "module/qtext/radiobutton.h"

namespace QtExt {

class NamedRadioButton : public RadioButton
{
  Q_OBJECT
  Q_DISABLE_COPY(NamedRadioButton)
  typedef NamedRadioButton Self;
  typedef RadioButton Base;

public:
  explicit NamedRadioButton(QWidget *parent = 0)
    : Base(parent) { init(); }

  explicit NamedRadioButton(const QString &text, QWidget *parent = 0)
    : Base(text, parent) { init(); }

signals:
  void toggledWithName(bool checked, const QString &title);

protected slots:
  void trigger(bool checked)
  { emit toggledWithName(checked, text()); }

private:
  void init()
  { connect(this, SIGNAL(toggled(bool)), SLOT(trigger(bool))); }
};

} // namespace QtExt

#endif // _QTEXT_NAMEDRADIOBUTTON_H
