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
  explicit CheckBox(QWidget *parent = nullptr)
    : Base(parent) { }

  explicit CheckBox(const QString &text, QWidget *parent = nullptr)
    : Base(text, parent) { }

  // - Events -
protected:
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
};

} // namespace QtExt

#endif // _QTEXT_CHECKBOX_H
