#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

// pagewidget.h
// 6/18/2011

#include "renderthread.h"
#include "core/page.h"
#include "core/document.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QRubberBand>
#include <QLabel>
#include <boost/algorithm/minmax.hpp>

namespace detail {

  ///  Vertical zoom factor
  inline qreal
  zoom_x(const QPaintDevice *paint, qreal zoom = 1.0)
  // Q_ASSERT(paint)
  { return zoom * paint->physicalDpiX() / Core::Document::dpiX(); }

  ///  Horizontal zoom factor
  inline qreal
  zoom_y(const QPaintDevice *paint, qreal zoom = 1.0)
  // Q_ASSERT(paint)
  { return zoom * paint->physicalDpiY() / Core::Document::dpiY(); }

  ///  Zoom factors in matrix
  inline QMatrix
  zoom_matrix(const QPaintDevice *paint, qreal zoom = 1.0)
  {
    return QMatrix(zoom_x(paint, zoom), 0.0,
                   0.0, zoom_y(paint, zoom),
                   0.0, 0.0);
  }

  ///  Convert to original size unit in touch document
  inline QRectF
  zoom_to_touch_unit(const QRectF &rect, QPaintDevice *paint, qreal zoom = 1.0)
  { return zoom_matrix(paint, zoom).inverted().mapRect(rect); }

  ///  Convert from original size unit in touch document
  inline QRectF
  zoom_from_touch_unit(const QRectF &rect, QPaintDevice *paint, qreal zoom = 1.0)
  { return zoom_matrix(paint, zoom).mapRect(rect); }

} // namespace detail

namespace detail {
  // PageRenderTask
  /**
   *  \brief  Render page into image.
   */
  class PageRenderTask : public RenderTask
  {
    Q_OBJECT
    typedef PageRenderTask Self;
    typedef RenderTask Base;

    Core::Page *page_;
    qreal xres_, yres_;

  public:
    PageRenderTask(Renderable *device, Core::Page *page, qreal xres, qreal yres, QObject *parent = 0)
      : Base(device, parent), page_(page), xres_(xres), yres_(yres) { }

    bool bad() const { return !device() || !page_ || page_->bad(); }

    virtual QImage render() const ///< Overriding.
    { return bad()? QImage() : page_->toImage(xres_, yres_); }

  };
} // namespace detail

// MouseRubberBand
/**
 *  \brief  Help control mouse using different states.
 */
class MouseRubberBand : public QRubberBand
{
  Q_OBJECT
  typedef MouseRubberBand Self;
  typedef QRubberBand Base;

  QPoint pressed_;

public:
  explicit MouseRubberBand(QWidget *parent = 0)
    : Base(Rectangle, parent) { }

  bool pressed() const
  { return !pressed_.isNull(); }

  QRectF selection() const
  { return QRectF(pos(), size()); }

public slots:
  void press(const QPoint &pos)
  {
    //resize(QSize());
    //move(pos);
    setGeometry(pos.x(), pos.y(), 0, 0);
    pressed_ = pos;
    show();
  }

  void move(const QPoint &pos)
  {
    int x1, y1, x2, y2;
    boost::tie(x1, x2) = boost::minmax(pos.x(), pressed_.x());
    boost::tie(y1, y2) = boost::minmax(pos.y(), pressed_.y());
    setGeometry(x1, y1, x2 - x1, y2 - y1);
  }

  void release()
  {
    hide();
    pressed_ = QPoint();
  }
};

// PagePreviewWidget
/**
 *  \brief  Page viewer.
 */
class PagePreviewWidget : public QGraphicsView, public Renderable
{
  Q_OBJECT
  typedef PagePreviewWidget Self;
  typedef QGraphicsView Base;

  Core::Page *page_;
  RenderThread *renderer_;
  qreal zoom_;
  MouseRubberBand *mouse_;

  QPixmap pixmap_; // backgouncd
  mutable QImage image_; // cached first rendered pixmap, supposed to be the same as that from toImage
  //QGraphicsPixmapItem *background_; // container of pixmap

public:
  explicit PagePreviewWidget(Core::Page *page, qreal zoom_ = 1.2,
                             RenderThread *renderer = 0, QWidget *parent = 0);

  bool bad() const { return !page_ || page_->bad(); }
  Core::Page *page() const { return page_; }
  //void setPage(Core::Page *page) { page_ = page; }

  ///  Return rendered image.
  QImage toImage() const
  {
    return bad() ? QImage()
         : page_->toImage(zoom_ * physicalDpiX(),  // xres
                          zoom_ * physicalDpiY()); // yres
  }

  RenderThread *renderThread() const { return renderer_; }
  void setRenderThread(RenderThread *thread) { renderer_ = thread; }

public slots:
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);

  // User select rect.
  void selectLines(const QRectF &rect);

public:
  virtual void setImage(const QImage &image) ///< Implement Renderable.
  {
    if (image_.isNull())
      image_ = image;

    setPixmap(QPixmap::fromImage(image));
  } // set size to trigger repaint

  void setPixmap(const QPixmap &pixmap)
  {
    pixmap_ = pixmap;
    //setFixedSize(pixmap_.size());
    repaintBackground();
  }

  const QPixmap *pixmap() const ///< Consistent with QLabel.
  { return &pixmap_; }

protected:
  void repaintBackground()
  { if (scene()) scene()->invalidate(QRectF(),QGraphicsScene::BackgroundLayer); }

  ///  Overridding base class.
  virtual void drawBackground(QPainter *painter, const QRectF &rect)
  {
    Base::drawBackground(painter, rect);
    if (!pixmap_.isNull())
      painter->drawPixmap(pixmap_.width() * -0.5, pixmap_.height() * -0.5, pixmap_);
  }
};

// PageThumbnailWidget
/**
 *  \brief  Display thumbnail of a page.
 */
class PageThumbnailWidget : public QLabel, public Renderable
{
  Q_OBJECT
  typedef PageThumbnailWidget Self;
  typedef QLabel Base;

  Core::Page *page_;
  RenderThread *renderer_;
  qreal zoom_;

public:
  explicit PageThumbnailWidget(Core::Page *page, qreal zoom_ = 2.0, RenderThread *renderer = 0, QWidget *parent = 0);

  bool bad() const { return !page_ || page_->bad(); }
  Core::Page *page() const { return page_; }
  //void setPage(Core::Page *page) { page_ = page; }

  ///  Return rendered image.
  QImage toImage() const;

  RenderThread *renderThread() const { return renderer_; }
  void setRenderThread(RenderThread *thread) { renderer_ = thread; }

  ///  Overiding render thread.
  virtual void setImage(const QImage &image)
  { setPixmap(QPixmap::fromImage(image)); }
};

#endif // PAGEWIDGET_H
