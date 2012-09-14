#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

// previewwidget.h
// 6/18/2011

#include "core/document.h"
#include <QWidget>
#include <QVector>

class PagePreviewWidget;
class RenderThread;

// PreviewWidget
/**
 *  \brief  Preview the entire document with pages.
 */
class PreviewWidget : public QWidget
{
  Q_OBJECT
  typedef PreviewWidget Self;
  typedef QWidget Base;

  Core::Document *document_;
  QVector<PagePreviewWidget*> pages_;
  RenderThread *renderer_;
  qreal zoom_;

public:
  explicit PreviewWidget(Core::Document *document, RenderThread *renderer = 0, QWidget *parent = nullptr);

  bool bad() const { return !document_ || document_->bad(); }
  Core::Document *document() const { return document_; }

  RenderThread *renderThread() const { return renderer_; }
  void setRenderThread(RenderThread *thread) { renderer_ = thread; }
};

#endif // PREVIEWWIDGET_H
