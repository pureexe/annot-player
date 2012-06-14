#ifndef _QTEXT_CHECKBOX_H
#define _QTEXT_CHECKBOX_H

// qtext/checkbox.h
// 5/30/2012

#include <QtGui/QCheckBox>

namespace QtExt {

class CheckBox : public QCheckBox
{
  Q_OBJECT
  Q_DISABLE_COPY(CheckBox)
  typedef CheckBox Self;
  typedef QCheckBox Base;

public:
  explicit CheckBox(QWidget *parent = 0)
    : Base(parent) { }

  explicit CheckBox(const QString &text, QWidget *parent = 0)
    : Base(text, parent) { }

  // - Events -
protected:
  virtual void enterEvent(QEvent *event); ///< \reimp
  virtual void leaveEvent(QEvent *event); ///< \reimp
};

} // namespace QtExt

#endif // _QTEXT_CHECKBOX_H
