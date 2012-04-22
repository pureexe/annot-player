#ifndef _QTEXT_WITHSIZEHINT_H
#define _QTEXT_WITHSIZEHINT_H

// core/gui/withsizehint.h
// 12/16/2011

#include <QtCore/QSize>

namespace QtExt {

class WithSizeHint
{
  QSize sizeHint_;

public:
  virtual ~WithSizeHint() { }

  void setSizeHint(const QSize &size) { sizeHint_ = size; }
  QSize sizeHint() const { return sizeHint_; }
};

} // namespace QtExt

#endif // _QTEXT_WITHSIZEHINT_H
