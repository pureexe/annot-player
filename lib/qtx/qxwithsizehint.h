#ifndef QXWITHSIZEHINT_H
#define QXWITHSIZEHINT_H

// qxwithsizehint.h
// 12/16/2011

#include "qtx/qtxglobal.h"
#include <QtCore/QSize>

QTX_BEGIN_NAMESPACE

class QxWithSizeHint
{
  QSize sizeHint_;

public:
  virtual ~QxWithSizeHint() { }

  void setSizeHint(const QSize &size) { sizeHint_ = size; }
  QSize sizeHint() const { return sizeHint_; }
};

QTX_END_NAMESPACE

#endif // QXWITHSIZEHINT_H
