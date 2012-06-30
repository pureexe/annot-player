// qtx.cc
// 11/25/2011

#include <QtGlobal>
#ifndef Q_WS_X11
# error "This file is for X11 only."
#endif // Q_WS_X11

#include "qtx/qtx.h"
#include <QtGui/QX11Info>
#include <QtCore>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>

// - Helper -

namespace { // anonymous, type-cast

  inline XRectangle
  XRectangleFromQRect(const QRect &qrect)
  {
    // { x, y, w, h }
    XRectangle ret = {
      (short)qrect.x(), (short)qrect.y(),
      (ushort)qrect.width(), (ushort)qrect.height()
    };
    return ret;
  }

  inline XserverRegion
  XserverRegionFromQRegion(const QRegion &qregion)
  {
    QVector<QRect> qrects = qregion.rects();
    int count = qrects.size();
    switch (count) {
      case 0: {
        XRectangle zero_rect = { 0, 0, 0, 0 };
        return ::XFixesCreateRegion(QX11Info::display(), &zero_rect, 1 );

      } case 1: {
        XRectangle xrect = XRectangleFromQRect(qrects.front());
        return ::XFixesCreateRegion(QX11Info::display(), & xrect, 1);

      } default: {
        XRectangle *xrects = new XRectangle[count];
        for (int i = 0; i < count; i++)
          xrects[i] = XRectangleFromQRect(qrects[i]);
        XserverRegion ret = ::XFixesCreateRegion(QX11Info::display(), xrects, count);
        delete[] xrects;
        return ret;
      }
    }
  }

} // anonymous namespace

// - X Windows -

void
QtX::zeroWindowInputShape(WId window)
{
  XRectangle zero_rect = { 0, 0, 0, 0 };
  XserverRegion empty_region = ::XFixesCreateRegion(QX11Info::display(), &zero_rect, 1 );
  ::XFixesSetWindowShapeRegion(QX11Info::display(), window, ShapeInput,
                               0, 0, empty_region);
}

void
QtX::setWindowInputShape(WId window, const QPoint &pos, const QRegion &region)
{
  if (region.isEmpty()) {
    zeroWindowInputShape(window);
    return;
  }
  ::XFixesSetWindowShapeRegion(QX11Info::display(), window, ShapeInput,
                               pos.x(), pos.y(), XserverRegionFromQRegion(region));
}

// - Keyboard -

// See: http://www.qtforum.org/article/32572/how-to-determine-if-capslock-is-on-crossplatform.html
bool
QtX::isKeyPressed(uint mask)
{
  bool ret = false;
  Display *d = ::XOpenDisplay(0);
  if (d) {
    uint n;
    if (::XkbGetIndicatorState(d, XkbUseCoreKbd, &n))
      ret = n & mask;
   }
   return ret;
}

bool
QtX::isKeyCapsLockPressed()
{ return isKeyPressed(0x01); }

// EOF
