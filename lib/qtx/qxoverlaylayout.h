#ifndef QXOVERLAYLAYOUT_H
#define QXOVERLAYLAYOUT_H

// qxoverlaylayout.h
// 4/8/2012

#include "qtx/qtxglobal.h"
#include <QtGui/QGridLayout>

QTX_BEGIN_NAMESPACE
class QxOverlayLayout : public QGridLayout
{
  Q_OBJECT
  Q_DISABLE_COPY(QxOverlayLayout)
  typedef QxOverlayLayout Self;
  typedef QGridLayout Base;

public:
  explicit QxOverlayLayout(QWidget *parent = nullptr)
    : Base(parent) { }

  void addWidget(QWidget *w, Qt::Alignment alignment = 0)
  { Base::addWidget(w, 0, 0, alignment); }

  void addLayout(QLayout *l, Qt::Alignment alignment = 0)
  { Base::addLayout(l, 0, 0, alignment); }
};

QTX_END_NAMESPACE

#endif // QXOVERLAYLAYOUT_H
