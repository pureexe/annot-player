#ifndef QXNAMEDCHECKBOX_H
#define QXNAMEDCHECKBOX_H

// qxnamedcheckbox.h
// 5/30/2012

#include "qtx/qxcheckbox.h"

QTX_BEGIN_NAMESPACE

class QxNamedCheckBox : public QxCheckBox
{
  Q_OBJECT
  Q_DISABLE_COPY(QxNamedCheckBox)
  typedef QxNamedCheckBox Self;
  typedef QxCheckBox Base;

public:
  explicit QxNamedCheckBox(QWidget *parent = nullptr)
    : Base(parent) { init(); }

  explicit QxNamedCheckBox(const QString &text, QWidget *parent = nullptr)
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

#endif // QXNAMEDCHECKBOX_H
