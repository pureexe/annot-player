#ifndef QXNAMEDRADIOBUTTON_H
#define QXNAMEDRADIOBUTTON_H

// qxnamedradiobutton.h
// 5/30/2012

#include "qtx/qxradiobutton.h"

QTX_BEGIN_NAMESPACE

class QxNamedRadioButton : public QxRadioButton
{
  Q_OBJECT
  Q_DISABLE_COPY(QxNamedRadioButton)
  typedef QxNamedRadioButton Self;
  typedef QxRadioButton Base;

public:
  explicit QxNamedRadioButton(QWidget *parent = nullptr)
    : Base(parent) { init(); }

  explicit QxNamedRadioButton(const QString &text, QWidget *parent = nullptr)
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

QTX_END_NAMESPACE

#endif // QXNAMEDRADIOBUTTON_H
