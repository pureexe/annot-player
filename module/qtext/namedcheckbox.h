#ifndef _QTEXT_NAMEDCHECKBOX_H
#define _QTEXT_NAMEDCHECKBOX_H

// qtext/namedcheckbox.h
// 5/30/2012

#include "module/qtext/checkbox.h"

namespace QtExt {

class NamedCheckBox : public CheckBox
{
  Q_OBJECT
  Q_DISABLE_COPY(NamedCheckBox)
  typedef NamedCheckBox Self;
  typedef CheckBox Base;

public:
  explicit NamedCheckBox(QWidget *parent = nullptr)
    : Base(parent) { init(); }

  explicit NamedCheckBox(const QString &text, QWidget *parent = nullptr)
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

#endif // _QTEXT_NAMEDCHECKBOX_H
