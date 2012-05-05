// pixmaphalofilter.cc
// 5/3/2012
#include "module/pixmapfilter/pixmaphalofilter.h"
#include <QtGui/QPainter>
#include <QtGui/QPaintEngine>

//#include "qpixmap.h"
//#include "qpixmapfilter_p.h"
//#include "qvarlengtharray.h"
//
//#include "private/qapplication_p.h"
//#include "private/qgraphicssystem_p.h"
//#include "private/qpaintengineex_p.h"
//#include "private/qpaintengine_raster_p.h"
//#include "qmath.h"
//#include "private/qmath_p.h"
//#include "private/qmemrotate_p.h"
//#include "private/qdrawhelper_p.h"

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
