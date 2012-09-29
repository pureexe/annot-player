#ifndef SKSTYLECLASS_H
#define SKSTYLECLASS_H

// skdeclarativewindow.h
// 9/22/2012

#include "sakurakit/skglobal.h"

SK_BEGIN_NAMESPACE

class SkStyleClass
{
  QString styleClass_;
public:
  QString styleClass() const { return styleClass_; }
  void setStyleClass(const QString &value) { styleClass_ = value; }
};

SK_END_NAMESPACE

#endif // SKSTYLECLASS_H
