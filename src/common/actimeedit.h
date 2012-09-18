#ifndef ACTIMEEDIT_H
#define ACTIMEEDIT_H

// actimeedit.h
// 5/30/2012

#include "qtx/qxtimeedit.h"

typedef QxTimeEdit AcTimeEditBase;
class AcTimeEdit : public AcTimeEditBase
{
  Q_OBJECT
  Q_DISABLE_COPY(AcTimeEdit)
  typedef AcTimeEdit Self;
  typedef AcTimeEditBase Base;

public:
  explicit AcTimeEdit(QWidget *parent = nullptr)
    : Base(parent) { init(); }

  explicit AcTimeEdit(const QTime &time, QWidget *parent = nullptr)
    : Base(time, parent) { init(); }

private:
  void init();
};

#endif // ACTIMEEDIT_H
