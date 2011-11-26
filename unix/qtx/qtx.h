#ifndef QTX_H
#define QTX_H

// qtx.h
// 11/25/2011

#include <QWidget>

namespace QtX {

  // - X Windows -

  void zeroWindowInputShape(WId window);
  void setWindowInputShape(WId window, const QPoint &pos, const QRegion &region);

} // namespace QtX

#endif // QTX_H
