#ifndef SKQUICKVIEW_H
#define SKQUICKVIEW_H

// skquickview.h
// 9/22/2012

#include "sakurakit/skglobal.h"
#include "qx/qxdraggablequickview.h"

SK_BEGIN_NAMESPACE

class SkQuickView : public QxDraggableQuickView
{
  Q_OBJECT
  Q_DISABLE_COPY(SkQuickView)
  QX_EXTEND_CLASS(SkQuickView, QxDraggableQuickView)

public:
  explicit SkQuickView(QWindow *parent = nullptr);

  bool isTranslucent() const;
  void setTranslucent(bool t);

  bool isFrameless() const;
  void setFrameless(bool t);
};

SK_END_NAMESPACE

#endif // SKQUICKVIEW_H
