#ifndef QXUIUTIL_H
#define QXUIUTIL_H

// qxuiutil.h
// 9/25/2012 jichi

#include "qx/qxglobal.h"
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QWindow>

QX_BEGIN_NAMESPACE

inline void qxCenterWindow(QWindow *w)
{
  Q_ASSERT(w);
  if (QScreen *screen = QGuiApplication::primaryScreen()) {
    QSize sz = screen->size() - w->size();
    w->setPos(sz.width()/2, sz.height()/2);
  }
}

QX_END_NAMESPACE

#endif // QXUIUTIL_H
