#ifndef QTX11_H
#define QTX11_H

// qtx11.h
// 11/25/2011

#include <QtGui/QWidget>

namespace QtX11 {

  // - Windows -

  void zeroWindowInputShape(WId window);
  void setWindowInputShape(WId window, const QPoint &pos, const QRegion &region);

  // - Keybaord -

  bool isKeyPressed(uint mask);
  bool isKeyCapsLockPressed();

} // namespace QtX11

#endif // QTX11_H
