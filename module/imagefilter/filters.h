#ifndef _IMAGEFILTER_FILTERS_H
#define _IMAGEFILTER_FILTERS_H

// imagefilter/filters.h
// 6/14/2012

#include <QtGlobal>

QT_FORWARD_DECLARE_CLASS(QPainter)
QT_FORWARD_DECLARE_CLASS(QImage)

void blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
void blurImage(QImage &blurImage, qreal radius, bool quality, int transposed = 0);

#endif _IMAGEFILTER_FILTERS_H
