// pixmaphalofilter.cc
// 5/3/2012
#include "module/pixmapfilter/pixmaphalofilter.h"
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QImage>

// - Construction -

PixmapHaloFilter::PixmapHaloFilter(QObject *parent)
  : Base(UserFilter, parent), radius_(0)
{ }


// - Paint -

void
PixmapHaloFilter::draw(QPainter *p, const QPointF &pos, const QPixmap &pm, const QRectF &src) const
{
  Q_ASSERT(p);
  QImage image = src.isEmpty() ? pm.toImage() :
                 pm.copy(src.toRect()).toImage();
  if (image.isNull())
    return;
  p->drawImage(pos, transform(image));
}

QImage&
PixmapHaloFilter::transform(QImage &image) const
{
  if (image.isNull())
    return image;

  QSize size = image.size();
  if (size.isEmpty())
    return image;

  int width = image.width(),
      height = image.height();
  QColor c;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      c = image.pixel(x, y);
      //c.setAlpha(0); // CHECKPOINT
      image.setPixel(x, y, c.rgba());
    }
  }

  return image;
}

// EOF
