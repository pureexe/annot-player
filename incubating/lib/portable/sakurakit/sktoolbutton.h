#ifndef SKTOOLBUTTON_H
#define SKTOOLBUTTON_H

// sktoolbutton.h
// 9/25/2012

#include "sakurakit/skglobal.h"
#include "qx/qxtoolbutton.h"

SK_BEGIN_NAMESPACE

class SkToolButton : public QxToolButton
{
  Q_OBJECT
  Q_DISABLE_COPY(SkToolButton)
  QX_EXTEND_CLASS(SkToolButton, QxToolButton)
  SK_SYNTHESIZE_STYLE_CLASS

public:
  explicit SkToolButton(QWidget *parent = nullptr)
    : Base(parent) { }
};

SK_END_NAMESPACE

#endif // SKTOOLBUTTON_H
