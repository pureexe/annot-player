#ifndef QXCOMPACTSTACKEDLAYOUT_H
#define QXCOMPACTSTACKEDLAYOUT_H

// qxcompactstackedlayout.h
// 5/9/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QStackedLayout>
#include <QtGui/QWidget>

QTX_BEGIN_NAMESPACE
class QxCompactStackedLayout : public QStackedLayout
{
  Q_OBJECT
  Q_DISABLE_COPY(QxCompactStackedLayout)
  typedef QxCompactStackedLayout Self;
  typedef QStackedLayout Base;

public:
  QxCompactStackedLayout() { }
  explicit QxCompactStackedLayout(QWidget *parent) : Base(parent) { }
  explicit QxCompactStackedLayout(QLayout *parentLayout) : Base(parentLayout) { }

  QSize sizeHint() const override
  {
    QWidget *w = currentWidget();
    return w ? w->sizeHint() : QSize();
  }
};

QTX_END_NAMESPACE

#endif // QXCOMPACTSTACKLAYOUT_H
