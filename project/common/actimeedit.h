#ifndef ACTIMEEDIT_H
#define ACTIMEEDIT_H

// actimeedit.h
// 5/30/2012

#include "module/qtext/timeedit.h"

class AcTimeEdit : public QtExt::TimeEdit
{
  Q_OBJECT
  Q_DISABLE_COPY(AcTimeEdit)
  typedef AcTimeEdit Self;
  typedef QtExt::TimeEdit Base;

public:
  explicit AcTimeEdit(QWidget *parent = nullptr)
    : Base(parent) { init(); }

  explicit AcTimeEdit(const QTime &time, QWidget *parent = nullptr)
    : Base(time, parent) { init(); }

private:
  void init();
};

#endif // ACTIMEEDIT_H
