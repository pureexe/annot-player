// skquickview.cc
// 9/22/2012

#include "sakurakit/skquickview.h"
#include "sakurakit/skui.h"
#include "qx/qxdeclarativeview.h"
#include <QtQuick/QQuickView>

SK_BEGIN_NAMESPACE

// - Construction -

SkQuickView::SkQuickView(QWindow *parent)
  : Base(parent)
{
  // Automatically resize qml object
  setResizeMode(SizeRootObjectToView);
}

void
SkQuickView::setTranslucent(bool t)
{
  SkUi::setAeroEffect(this, t);

  if (t) {
    // See: https://bugreports.qt-project.org/browse/QTBUG-20768
    QSurfaceFormat fmt;
    fmt.setAlphaBufferSize(8);
    setFormat(fmt);
    //setClearBeforeRendering(true);
    setColor(Qt::transparent);
  } else
    setColor(QColor());
}

bool
SkQuickView::isTranslucent() const
{ return color() == Qt::transparent; }

void
SkQuickView::setFrameless(bool t)
{
  setWindowFlags(t ? (windowFlags() | Qt::Popup)
                   : (windowFlags() & ~Qt::Popup));
}

bool
SkQuickView::isFrameless() const
{ return windowFlags() & (Qt::Popup | Qt::FramelessWindowHint); }

SK_END_NAMESPACE
