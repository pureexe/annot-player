#ifndef PIXMAPFILTER_H
#define PIXMAPFILTER_H

// pixmapfilter.h
// 5/3/2012

#include <gui/image/qpixmapfilter_p.h>

QT_BEGIN_NAMESPACE
Q_GUI_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
Q_GUI_EXPORT void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed = 0);
QT_END_NAMESPACE

#endif // PIXMAPFILTER_H
