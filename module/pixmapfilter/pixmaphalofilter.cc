// pixmaphalofilter.cc
// 5/3/2012
#include "module/pixmapfilter/pixmaphalofilter.h"
#include <QtGui/QPainter>
#include <QtGui/QPaintEngine>

// See: gui/image/qpixmapfilter_p.h
QT_BEGIN_NAMESPACE
Q_GUI_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
Q_GUI_EXPORT void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed = 0);
QT_END_NAMESPACE


PixmapHaloFilter::PixmapHaloFilter(QObject *parent)
  : Base(UserFilter, parent),
    offset_(8, 8), color_(63, 63, 63, 180), radius_(1)
{ }


QRectF
PixmapHaloFilter::boundingRectFor(const QRectF &rect) const
{ return rect.united(rect.translated(offset_).adjusted(-radius_, -radius_, radius_, radius_)); }

void
PixmapHaloFilter::draw(QPainter *p, const QPointF &pos, const QPixmap &px, const QRectF &src) const
{
  if (px.isNull())
    return;

  QImage tmp(px.size(), QImage::Format_ARGB32_Premultiplied);
  tmp.fill(0);
  QPainter tmpPainter(&tmp);
  tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
  //if (!offset_.isNull())
  //  tmpPainter.drawPixmap(-offset_, px); // surrounded halo
  tmpPainter.drawPixmap(offset_, px);
  tmpPainter.end();

  // blur the alpha channel
  QImage blurred(tmp.size(), QImage::Format_ARGB32_Premultiplied);
  blurred.fill(0);
  QPainter blurPainter(&blurred);
  qt_blurImage(&blurPainter, tmp, radius_, false, true);
  blurPainter.end();

  // blacken the image...
  QPainter blackenPainter(&blurred);
  blackenPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  blackenPainter.fillRect(blurred.rect(), color_);
  blackenPainter.end();

  // draw the blurred drop shadow...
  p->drawImage(pos, blurred);

  // Draw the actual pixmap...
  p->drawPixmap(pos, px, src);
}

// EOF
