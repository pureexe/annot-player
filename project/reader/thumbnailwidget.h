#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

// thumbnailwidget.h
// 6/18/2011

#include "core/document.h"
#include <QWidget>
#include <QVector>

class PageThumbnailWidget;
class RenderThread;

// ThumbnailWidget
/**
 *  \brief  Preview the entire document with thumbnails of pages.
 */
class ThumbnailWidget : public QWidget
{
  Q_OBJECT
  typedef ThumbnailWidget Self;
  typedef QWidget Base;

  Core::Document *document_;
  QVector<PageThumbnailWidget*> pages_;
  RenderThread *renderer_;
  qreal zoom_;

public:
  explicit ThumbnailWidget(Core::Document *document, RenderThread *renderer_ = 0, QWidget *parent = nullptr);

  bool bad() const { return !document_ || document_->bad(); }
  Core::Document *document() const { return document_; }

  RenderThread *renderThread() const { return renderer_; }
  void setRenderThread(RenderThread *thread) { renderer_ = thread; }
};

#endif // THUMBNAILWIDGET_H
