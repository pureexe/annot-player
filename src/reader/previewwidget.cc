// preivewwidget.cc
// 6/18/2011

#include "previewwidget.h"
#include "renderthread.h"
#include "pagewidget.h"
#include <QtGui>

#define DEFAULT_PREVIEW_ZOOM    1.2

/**
 * class PreviewWidget
 */
PreviewWidget::PreviewWidget(Core::Document *document, RenderThread *renderer, QWidget *parent)
  : Base(parent), document_(document), renderer_(renderer), zoom_(DEFAULT_PREVIEW_ZOOM)
{
  if (document_ && !document_->bad()) {
    QString title = tr("Preview - ") + document->title();
    setWindowTitle(title);

    int num_pages = document_->pageCount();
    Q_ASSERT(num_pages > 0);
    //pages_.clear();
    pages_.resize(num_pages);

    QLayout *layout = new QVBoxLayout(this);
    // Always display page 0 in main thread
    pages_[0] = new PagePreviewWidget(document->page(0), zoom_, 0, this);
    pages_[0]->setRenderThread(renderer);
    layout->addWidget(pages_[0]);

    // page 1 ~ left
    for (int i = 1; i < num_pages; i++) {
      pages_[i] = new PagePreviewWidget(document->page(i), zoom_, renderer_, this);
      layout->addWidget(pages_[i]);
    }
    setLayout(layout);
  }
}

// EOF
