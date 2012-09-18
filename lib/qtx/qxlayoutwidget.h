#ifndef QXLAYOUTWIDGET_H
#define QXLAYOUTWIDGET_H

// qxlayoutwidget.h
// 4/8/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QWidget>

QTX_BEGIN_NAMESPACE

class QxLayoutWidget : public QWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(QxLayoutWidget)
  typedef QxLayoutWidget Self;
  typedef QWidget Base;

public:
  explicit QxLayoutWidget(QLayout *layout, QWidget *parent = nullptr)
    : Base(parent)
  {
    setLayout(layout);
    setContentsMargins(0, 0, 0, 0);
  }
};

QTX_END_NAMESPACE

#endif // QXLAYOUTWIDGET_H
