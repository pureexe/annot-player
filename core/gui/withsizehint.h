#ifndef _CORE_GUI_WITHSIZEHINT_H
#define _CORE_GUI_WITHSIZEHINT_H

// core/gui/withsizehint.h
// 12/16/2011

#include <QSize>

namespace Core { namespace Gui {

class WithSizeHint
{
  QSize sizeHint_;

public:
  virtual ~WithSizeHint() { }

  void setSizeHint(const QSize &size) { sizeHint_ = size; }
  QSize sizeHint() const { return sizeHint_; }
};

} } // namespace Gui, Core

#endif // _CORE_GUI_WITHSIZEHINT_H
