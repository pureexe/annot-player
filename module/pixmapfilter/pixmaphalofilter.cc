// pixmaphalofilter.cc
// 5/3/2012
// See: gui/image/pixmapfilter_p.cpp

#include "module/pixmapfilter/pixmaphalofilter.h"
#include "module/imagefilter/filters.h"
#include <gui/painting/qpaintengineex_p.h>
#include <QtGui>

// - Construction -

PixmapHaloFilter::PixmapHaloFilter(QObject *parent)
  : Base(UserFilter, parent), offset_(8, 8), color_(63, 63, 63, 180), radius_(1)
{ }

// - Properties -

QRectF
PixmapHaloFilter::boundingRectFor(const QRectF &rect) const
{ return rect.united(rect.translated(offset_).adjusted(-radius_, -radius_, radius_, radius_)); }

// - Paint -

void
PixmapHaloFilter::draw(QPainter *p, const QPointF &pos, const QPixmap &px, const QRectF &src) const
{
  if (px.isNull())
    return;

  QPaintEngine *e = p->paintEngine();
  Base *filter = e && e->isExtended() ?
        static_cast<QPaintEngineEx *>(e)->pixmapFilter(type(), this) :
        0;
  Self *haloFilter = static_cast<Self *>(filter);
  if (haloFilter) {
    haloFilter->setColor(color_);
    haloFilter->setBlurRadius(radius_);
    haloFilter->setOffset(offset_);
    haloFilter->draw(p, pos, px, src);
    return;
  }

  QImage tmp(px.size(), QImage::Format_ARGB32_Premultiplied);
  tmp.fill(0);
  QPainter tmpPainter(&tmp);
  tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
  tmpPainter.drawPixmap(offset_, px);
  tmpPainter.end();

  // blur the alpha channel
  QImage blurred(tmp.size(), QImage::Format_ARGB32_Premultiplied);
  blurred.fill(0);
  QPainter blurPainter(&blurred);
  ::blurImage(&blurPainter, tmp, radius_, false, true);
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
