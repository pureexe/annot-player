// documentwidget.cc
// 6/18/2011

#include "documentwidget.h"
#include "renderthread.h"
#include "pagewidget.h"
#include "annotationwidget.h"
#include "previewwidget.h"
#include "thumbnailwidget.h"
#include "catalogwidget.h"
#include <QtGui>
/**
 * class DocumentWidget
 */

DocumentWidget::DocumentWidget(const QString &filePath, bool parallel, QWidget *parent)
  : Base(parent), document_(0), catalog_(0), preview_(0), thumbnail_(0), renderer_(0)
{
  setContentsMargins(0, 0, 0, 0);

  document_ = new Core::Document(filePath, this);
  if (document_->bad()) {
    delete document_;
    document_ = 0;
    return;
  }

  QString title = document_->title();
  if (title.isEmpty())
    title = document_->subject();
  if (title.isEmpty())
    title = document_->fileName();
  if (title.isEmpty())
    title = tr("New document");
  setWindowTitle(title);

  if (parallel)
    createThreads();
  createDockWindows();
}

void
DocumentWidget::createThreads()
{
  renderer_ = new RenderThread(this);
  connect(renderer_, SIGNAL(renderableDevice(Renderable*, QImage)),
          SLOT(renderDevice(Renderable*, QImage)));
}

void
DocumentWidget::createDockWindows()
{
  if (!document_)
    return;

  { // Right size docks
    documentAnnot_ = new DocumentAnnotationWidget(this);
    QDockWidget *documentAnnotDock = new QDockWidget(this);
    documentAnnotDock->setTitleBarWidget(new QWidget(documentAnnotDock));
    documentAnnotDock->setWindowTitle(documentAnnot_->windowTitle());
    QScrollArea *documentAnnotArea = new QScrollArea(documentAnnotDock);
    documentAnnotArea->setWidget(documentAnnot_);
    documentAnnotDock->setWidget(documentAnnotArea);
    addDockWidget(Qt::RightDockWidgetArea, documentAnnotDock);

    pageAnnot_ = new PageAnnotationWidget(this);
    QDockWidget *pageAnnotDock = new QDockWidget(this);
    pageAnnotDock->setTitleBarWidget(new QWidget(pageAnnotDock));
    pageAnnotDock->setWindowTitle(pageAnnot_->windowTitle());
    QScrollArea *pageAnnotArea = new QScrollArea(pageAnnotDock);
    pageAnnotArea->setWidget(pageAnnot_);
    pageAnnotDock->setWidget(pageAnnotArea);
    addDockWidget(Qt::RightDockWidgetArea, pageAnnotDock);

    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);
    tabifyDockWidget(pageAnnotDock, documentAnnotDock);
  }


  { // Left side docks:

    thumbnail_ = new ThumbnailWidget(document_, renderer_, this);
    QDockWidget *thumbnailDock = new QDockWidget(this);
    thumbnailDock->setWindowTitle(thumbnail_->windowTitle());
    thumbnailDock->setTitleBarWidget(new QWidget(thumbnailDock));
    QScrollArea *thumbnailArea = new QScrollArea(thumbnailDock);
    thumbnailArea->setWidget(thumbnail_);
    thumbnailDock->setWidget(thumbnailArea);
    //thumbnailDock->setTitleBarWidget(new QWidget(thumbnailDock));
    //thumbnailDock->setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::LeftDockWidgetArea, thumbnailDock);

    preview_ = new PreviewWidget(document_, renderer_, this);
    QScrollArea *previewArea = new QScrollArea(this);
    previewArea->setWidget(preview_);
    setCentralWidget(previewArea);

    QDockWidget *catalogDock = 0;
    if (document_->catalog()) {
      catalog_ = new CatalogWidget(document_->catalog(), this);
      catalogDock = new QDockWidget(this);
      catalogDock->setTitleBarWidget(new QWidget(catalogDock));
      catalogDock->setWindowTitle(catalog_->windowTitle());
      //QScrollArea *catalogArea = new QScrollArea(catalogDock);
      //catalogArea->setWidget(catalog_);
      catalogDock->setWidget(catalog_);
      //catalogDock->setTitleBarWidget(new QWidget(catalogDock));
      //catalogDock->setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetClosable);
      addDockWidget(Qt::LeftDockWidgetArea, catalogDock);
    }

    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);
    if (thumbnailDock && catalogDock)
      tabifyDockWidget(thumbnailDock, catalogDock);
  }

  if (renderer_)
    renderer_->start();
}


void
DocumentWidget::renderDevice(Renderable *device, const QImage &image)
{ if (device) device->setImage(image); }

// EOF
/*
void PageWidget::selectBlocks(const QRectF &rect)
{
  QRectF t_select = ::zoom_to_touch_unit(rect, this, zoom_);

  typedef QList<Poppler::TextBox*> T;
  T l = page_->impl()->textList();
  T::const_iterator first = l.end(),
                    last = l.end();
  for (T::const_iterator cur = l.begin(); cur != l.end(); ++cur)
    if (t_select.intersects((*cur)->boundingBox()))
      if (first == l.end())
        first = last = cur;
      else
        last = cur;

  if (first == l.end())
    return;

  QImage image = toImage();
  QPainter painter;
  painter.begin(&image);

  BOOST_FOREACH (Poppler::TextBox *b, std::make_pair(first, last + 1)) {
    QRectF t_highlight = b->boundingBox();
    if (!t_highlight.isEmpty()) {
      QRectF highlight = ::zoom_from_touch_unit(t_highlight, this, zoom_);
      //highlight.adjust(-2, -2, 2, 2);
      painter.fillRect(highlight, SELECT_COLOR);
    }
  }
  painter.end();
  setPixmap(QPixmap::fromImage(image));

  QString text;
  bool hadSpace = false;
  QPointF center;
  if (hadSpace)
    text += " ";
    if (!text.isEmpty() &&
      box->boundingBox().top() > center.y())
       text += "\n";

     text += box->text();
    hadSpace = box->hasSpaceAfter();
    center = box->boundingBox().center();

  if (!text.isEmpty())
    emit textSelected(text);
}
*/

/**
 * class DocumentRenderThread
 */
/*
DocumentRenderThread::DocumentRenderThread(const Poppler::Document *toRender,
                                           DocumentRenderWidget *parent)
  : QThread(parent), document(toRender)
{ }

DocumentRenderThread::DocumentRenderThread(DocumentRenderWidget *parent)
  : QThread(parent), document(0)
{ }

void DocumentRenderThread::setDocument(const Poppler::Document *toRender)
{ document = toRender; }

void DocumentRenderThread::run()
{
  DocumentRenderWidget *widget = dynamic_cast<DocumentRenderWidget*>(parent());
  if (!widget || !document)
    return;

  qreal zoom = widget->zoomFactor();
  qreal x = zoom * widget->physicalDpiX(),
        y = zoom * widget->physicalDpiY();
  for (int i = 0; i < document->numPages(); i++) {
    QImage image = document->page(i)->renderToImage(x, y);
    emit pageReady(i, image);
  }
}
*/


/*
void DocumentRenderWidget::createActions()
{
  zoomInAct = new QAction(QIcon(":/image/zoomIn.png"), tr("Zoom &In"), this);
  zoomInAct->setShortcuts(QKeySequence::ZoomIn);
  zoomInAct->setStatusTip(tr("Zoom in"));
  connect(zoomInAct, SIGNAL(triggered()), SLOT(zoomIn()));

  zoomOutAct = new QAction(QIcon(":/image/zoomOut.png"), tr("Zoom &Out"), this);
  zoomOutAct->setShortcuts(QKeySequence::ZoomOut);
  zoomOutAct->setStatusTip(tr("Zoom out"));
  connect(zoomOutAct, SIGNAL(triggered()), SLOT(zoomOut()));
}

void DocumentRenderWidget::createContextMenu()
{
  addAction(zoomInAct);
  addAction(zoomOutAct);
  setContextMenuPolicy(Qt::ActionsContextMenu);
}

void DocumentRenderWidget::createThreads()
{
  renderThread = new DocumentRenderThread(this);
  connect(renderThread, SIGNAL(pageReady(int, QImage)),
          this, SLOT(renderPage(int, QImage)));

  if (document)
    renderThread->setDocument(document);
}

void DocumentRenderWidget::createPages()
{
  if (!document)
    return;

  int num_pages = document->numPages();
  assert(num_pages > 0);
  pages.reset(new QLabel[num_pages]);

  QLayout *layout = new QVBoxLayout(this);
  for (int i = 0; i < num_pages; i++)
    layout->addWidget(&pages[i]);
  setLayout(layout);
}

void DocumentRenderWidget::setZoomFactor(qreal rate)
{
  if (rate <= 0)
    return;

  zoom = rate;
  renderPages();
}

void DocumentRenderWidget::setDocument(Poppler::Document *toRender)
{
  document = toRender;
  if (document) {
    assert(renderThread);
    renderThread->setDocument(document);
    renderDocument();
  }
}

void DocumentRenderWidget::renderDocument()
{
  if (!document)
    return;

  setCursor(Qt::ArrowCursor);

  document->setRenderHint(Poppler::Document::Antialiasing);
  document->setRenderHint(Poppler::Document::TextAntialiasing);

  // TODO use document->infoKeys to get all meta data
  // CreationDate // Creator
  // ModDate
  // PTEX.Fullbanner
  // Producer
  // Trapped
  // Title
  // Author
  // Keywords
  //QStringList l = document->infoKeys();
  //std::cerr << "PDF infoKeys:" <<std::endl;
  //for (QStringList::const_iterator p = l.begin(); p != l.end(); ++p)
  //  std::cerr << p->toStdString() << std::endl;

  //QDateTime createdTime = document->date("CreationTime");
  //QDateTime modifiedTime = document->date("ModDate");
  QString title = document->info("Title");
  QString subject = document->info("Subject");

  if (title.isEmpty())
    title = subject;
  if (title.isEmpty())
    title = tr("Document");
  setWindowTitle(title);

  createPages();
  renderPages();
}

void DocumentRenderWidget::clear()
{
  document = 0;
  pages.reset();
  delete layout();
}
*/

/*
void DocumentRenderWidget::renderPage(int number, const QImage& image)
{
  renderMutex.lock();
  if (!document)
    return;

  int num_pages = document->numPages();
  assert(0 <= number && number <= num_pages);

  QPixmap pixmap = QPixmap::fromImage(image);
  pages[number].setPixmap(pixmap);
  //pageLinks = document -> page(i) -> links();

  //QSize size = pixmap.size();
  //pages[number].setFixedSize(size.width() * zoom, size.height() * zoom);
  renderMutex.unlock();
}

void DocumentRenderWidget::renderPages(bool parallel)
{
  renderMutex.lock();
  if (!document)
    return;

  int num_pages = document->numPages();

  // TODO 6/22/2011: Set size hint rather than the real size.
  for (int i = 0; i < num_pages; i++) {
    QSize size = document->page(i)->pageSize();
    pages[i].setFixedSize(size.width() * zoom, size.height() * zoom);
  }

  if (parallel) {
    assert(renderThread);
    if (renderThread->isRunning())
      renderThread->terminate();

    renderMutex.unlock();
    renderThread->start();

  } else {
    renderMutex.unlock();

    qreal x = zoom * physicalDpiX(),
          y = zoom * physicalDpiY();
    for (int i = 0; i < document->numPages(); i++) {
      QImage image = document->page(i)->renderToImage(x, y);
      renderPage(i, image);
    }
  }
}

void DocumentRenderWidget::zoomIn()
{
  zoom *= 1.2;
  renderPages();
}

void DocumentRenderWidget::zoomOut()
{
  zoom /= 1.2;
  renderPages();
}
*/

/*
void DocumentRenderWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
  Base::mouseDoubleClickEvent(event);
  emit mouseDoubleClicked(event);
}
*/

