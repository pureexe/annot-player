// qxdeclarativeview.cc
// 9/22/2012

#include "qx/qxdeclarativeview.h"
#include <QtQuick/QQuickView>

QX_BEGIN_NAMESPACE

// - Construction -

QxDeclarativeView::QxDeclarativeView(QWidget *parent)
  : Base(parent)
{ setDelegateWindow(new QQuickView(windowHandle())); }

QxDeclarativeView::QxDeclarativeView(const QUrl &source, QWidget *parent)
  : Base(parent)
{ setDelegateWindow(new QQuickView(source, windowHandle())); }

QxDeclarativeView::QxDeclarativeView(QQmlEngine *engine, QWidget *parent)
  : Base(parent)
{ setDelegateWindow(new QQuickView(engine, windowHandle())); }

// - Properties -

QQuickView*
QxDeclarativeView::qmlView() const
{ return static_cast<QQuickView *>(Base::delegateWindow()); }

void
QxDeclarativeView::setQmlView(QQuickView *view)
{ Base::setDelegateWindow(view); }

QQmlEngine*
QxDeclarativeView::engine() const
{
  QQuickView *p = qmlView();
  return p ? p->engine() : nullptr;
}

QUrl
QxDeclarativeView::source() const
{
  QQuickView *p = qmlView();
  return p ? p->source() : QUrl();
}

void
QxDeclarativeView::setSource(const QUrl &url)
{
  if (QQuickView *p = qmlView())
    p->setSource(url);
}

/*
void
QxDeclarativeView::init()
{
  setWindowFlags(Qt::FramelessWindowHint);
  QSurfaceFormat fmt = format();
  fmt.setAlphaBufferSize(8);
  setFormat(fmt);
  setClearBeforeRendering(true);
  setColor(Qt::transparent);
}
*/

QX_END_NAMESPACE
