// thumbnailwidget.cc
// 6/18/2011

#include "thumbnailwidget.h"
#include "renderthread.h"
#include "pagewidget.h"
#include <QtGui>

// jichi 7/6/2011: Note, this number cannot be so small (like 0.2) in mac.
// Or there will be a "bogus memory" error from Poppler on mac.
#ifdef Q_OS_MAC
  #define DEFAULT_THUMBNAIL_ZOOM  0.2
#else
  #define DEFAULT_THUMBNAIL_ZOOM  0.2
#endif // Q_OS_MAC

/**
 * class ThumbnailWidget
 */
ThumbnailWidget::ThumbnailWidget(Core::Document *document, RenderThread *renderer, QWidget *parent)
  : Base(parent), document_(document), renderer_(renderer), zoom_(DEFAULT_THUMBNAIL_ZOOM)
{
  if (document_ && !document_->bad()) {
    QString title = tr("Thumbnail");
    setWindowTitle(title);

    int num_pages = document_->pageCount();
    Q_ASSERT(num_pages > 0);
    //pages_.clear();
    pages_.resize(num_pages);

    QLayout *layout = new QVBoxLayout(this);
    for (int i = 0; i < num_pages; i++) {
      pages_[i] = new PageThumbnailWidget(document->page(i), zoom_, renderer_, this);
      layout->addWidget(pages_[i]);
    }
    setLayout(layout);
  }
}

// EOF
