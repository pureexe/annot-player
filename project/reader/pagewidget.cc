// pagewidget.cc
// 6/18/2011

#include "pagewidget.h"
#include <QtGui>
#include <boost/foreach.hpp>
#include <utility>

#define COLOR_SELECT            QColor(0, 0, 255, 63)
#define COLOR_SELECT_LINE1      QColor(0, 0, 255, 63)
#define COLOR_SELECT_LINE2      QColor(0, 0, 255, 127)


/**
 * class PagePreviewWidget
 */

PagePreviewWidget::PagePreviewWidget(Core::Page *page, qreal zoom, RenderThread *renderer, QWidget *parent)
  : Base(parent), page_(page), renderer_(renderer), zoom_(zoom), mouse_(0)
{
  setScene(new QGraphicsScene(this));

  if (page_ && !page_->bad()) {
    QSizeF sizeF = page_->sizeF();
    sizeF.rwidth() *= detail::zoom_x(this, zoom_);
    sizeF.rheight() *= detail::zoom_y(this, zoom_);
    setFixedSize(sizeF.toSize());

    if (renderer_) {
      double xres = zoom_ * physicalDpiX(),
             yres = zoom_ * physicalDpiY();
      renderer_->addTask(new detail::PageRenderTask(this, page_, xres, yres, renderer_));

    } else
      setImage(toImage());
  }
}

/**
 * Events
 */

void
PagePreviewWidget::mousePressEvent(QMouseEvent *event)
{
  if (event && event->button() == Qt::LeftButton) {
    if (!mouse_)
      mouse_ = new MouseRubberBand(this);

   mouse_->press(event->pos());
   event->accept();
  }
}

void
PagePreviewWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (event)
    if (mouse_ && mouse_->pressed()) {
      mouse_->move(event->pos());

      QRectF rect = mouse_->selection();
      if (pixmap())
        rect.moveTo(rect.x() - (width() - pixmap()->width()) / 2.0,
                    rect.y() - (height() - pixmap()->height()) / 2.0); // patching
      selectLines(rect);

      event->accept();
    }
}

void
PagePreviewWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if (mouse_ && mouse_->pressed()) {
    mouse_->release();
    if (event)
      event->accept();
  }
}

void PagePreviewWidget::selectLines(const QRectF &rect)
{
  QRectF t_select = detail::zoom_to_touch_unit(rect, this, zoom_);

  typedef Core::TextRectList T;
  const T &l = page_->lines();
  T::const_iterator first = l.end(),
                    last = l.end();
  for (T::const_iterator cur = l.begin(); cur != l.end(); ++cur)
    if (t_select.intersects(**cur)) {
      if (first == l.end())
        first = last = cur;
      else
        last = cur;
    }

  if (first == l.end())
    return;

  // FIXME: use a smaller rect to apply painter, for performance
  QImage image = image_;
  QPainter painter;
  if (!painter.begin(&image))
    return;

  BOOST_FOREACH (Core::TextRect *r, std::make_pair(first, last + 1)) {
    QRectF t_highlight = *r;
    if (!t_highlight.isEmpty()) {
      QRectF highlight = detail::zoom_from_touch_unit(t_highlight, this, zoom_);
      highlight.adjust(-2, -2, 2, 2);
      painter.fillRect(highlight, COLOR_SELECT);
    }
  }
  painter.end();
  setImage(image);
}

/**
 * class PageThumbnailWidget
 */

PageThumbnailWidget::PageThumbnailWidget(Core::Page *page, qreal zoom, RenderThread *renderer, QWidget *parent)
  : Base(parent), page_(page), renderer_(renderer), zoom_(zoom)
{
  if (page_ && !page_->bad()) {
    QSizeF sizeF = page_->sizeF();
    sizeF.rwidth() *= detail::zoom_x(this, zoom_);
    sizeF.rheight() *= detail::zoom_y(this, zoom_);
    setFixedSize(sizeF.toSize());

    if (renderer_) {
      double xres = zoom_ * physicalDpiX(),
             yres = zoom_ * physicalDpiY();
      renderer_->addTask(new detail::PageRenderTask(this, page_, xres, yres, renderer_));
    } else
      setImage(toImage());
  }
}

QImage
PageThumbnailWidget::toImage() const
{
  if (bad())
    return QImage();

  qreal xres = zoom_ * physicalDpiX(),
        yres = zoom_ * physicalDpiY();
  return page_->toImage(xres, yres);

  /*
  QImage image = page_->impl()->thumbnail();
  if (image.isNull())
    return QImage();

  if (zoom_ < 1.01 && zoom_ > 0.99)
    return image;

  return image.scaled(image.size() * zoom_);
  */
}

// EOF
