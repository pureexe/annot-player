#ifndef QXDECLARATIVEVIEW_H
#define QXDECLARATIVEVIEW_H

// qxdeclarativeview.h
// 9/22/2012

#include "qx/qxwindowcontainer.h"

QT_FORWARD_DECLARE_CLASS(QQmlEngine)
QT_FORWARD_DECLARE_CLASS(QQuickView)

QX_BEGIN_NAMESPACE

class QxDeclarativeView : public QxWindowContainer
{
  Q_OBJECT
  Q_DISABLE_COPY(QxDeclarativeView)
  QX_EXTEND_CLASS(QxDeclarativeView, QxWindowContainer)

public:
  explicit QxDeclarativeView(QWidget *parent = nullptr);
  explicit QxDeclarativeView(const QUrl &source, QWidget *parent = nullptr);
  explicit QxDeclarativeView(QQmlEngine *engine, QWidget *parent = nullptr);

public:
  QQuickView *qmlView() const;
  void setQmlView(QQuickView *value);

  QQmlEngine *engine() const;
  QUrl source() const;
public slots:
  void setSource(const QUrl &url);
};

QX_END_NAMESPACE

#endif // QXDECLARATIVEVIEW_H
