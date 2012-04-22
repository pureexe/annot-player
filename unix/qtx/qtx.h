#ifndef QTX_H
#define QTX_H

// qtx.h
// 11/25/2011

#include <QtGui/QWidget>

namespace QtX {

  // - Windows -

  void zeroWindowInputShape(WId window);
  void setWindowInputShape(WId window, const QPoint &pos, const QRegion &region);

  // - Keybaord -

  bool isKeyPressed(uint mask);
  bool isKeyCapsLockPressed();

} // namespace QtX

#endif // QTX_H
