// qtext/webview.cc
// 10/9/2011

#include "module/qtext/webview.h"
#include "module/qtext/webpage.h"
#ifdef WITH_MODULE_DOWNLOAD
#  include "module/qtext/filesystem.h"
#  include "module/download/download.h"
#endif // WITH_MODULE_DOWNLOAD
#include <QtGui>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>
#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

//#define DEBUG "qtext::webview"
#include "module/debug/debug.h"

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around assignment
#endif // __GNUC__

//#define WINDOW_FLAGS
//  Qt::Dialog |
//  Qt::CustomizeWindowHint |
//  Qt::WindowTitleHint |
//  Qt::WindowSystemMenuHint |
//  Qt::WindowMinMaxButtonsHint |
//  Qt::WindowCloseButtonHint

#define TEXT_SIZE_SCALE 0.85
#define ZOOM_MAX        5.0
#define ZOOM_MIN        0.5

#define SAVE_PATH       QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);

//#ifdef Q_WS_MAC
//#  define K_CTRL        "cmd"
//#else
//#  define K_CTRL        "Ctrl"
//#endif // Q_WS_MAC

QtExt::
WebView::WebView(QWidget *parent)
  : Base(parent), loading_(false)
{
  //setWindowFlags(f ? f : WINDOW_FLAGS);

  setTextSizeMultiplier(TEXT_SIZE_SCALE);

  // Antialias and smooth pixel is not turned on to improve performance.
  setRenderHints(QPainter::TextAntialiasing);

  WebPage *page = new WebPage(this); {
    connect(page, SIGNAL(linkHovered(QString,QString,QString)), SLOT(showLink(QString)));
    connect(page, SIGNAL(openLinkRequested(QString)), SIGNAL(openLinkRequested(QString)));
    connect(page, SIGNAL(downloadRequested(QNetworkRequest)), SLOT(download(QNetworkRequest)));

    connect(page, SIGNAL(loadStarted()), SLOT(setLoading()));
    connect(page, SIGNAL(loadFinished(bool)), SLOT(setFinished()));
    //connect(this, SIGNAL(loadStarted()), SLOT(setLoading()));
    //connect(this, SIGNAL(loadFinished(bool)), SLOT(setFinished()));
  } setPage(page);

  createActions();

}

void
QtExt::
WebView::createActions()
{
  clipAct = new QAction(this); {
    clipAct->setText(tr("Clip the page"));
    clipAct->setStatusTip(tr("Save the web page as image to disk and clipboard"));
    clipAct->setShortcut(QKeySequence::Save);
    connect(new QShortcut(QKeySequence::Save, this), SIGNAL(activated()),
            clipAct, SLOT(trigger()));
  } connect(clipAct, SIGNAL(triggered()), SLOT(clip()));

  zoomInAct = new QAction(this); {
    zoomInAct->setText(tr("Zoom in"));
    zoomInAct->setStatusTip(tr("Zoom in"));
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    connect(new QShortcut(QKeySequence("CTRL+="), this), SIGNAL(activated()),
            zoomInAct, SLOT(trigger()));
  } connect(zoomInAct, SIGNAL(triggered()), SLOT(zoomIn()));

  zoomOutAct = new QAction(this); {
    zoomOutAct->setText(tr("Zoom out"));
    zoomOutAct->setStatusTip(tr("Zoom out"));
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    connect(new QShortcut(QKeySequence("CTRL+-"), this), SIGNAL(activated()),
            zoomOutAct, SLOT(trigger()));
  } connect(zoomOutAct, SIGNAL(triggered()), SLOT(zoomOut()));

  zoomResetAct = new QAction(this); {
    zoomResetAct->setText(tr("Reset zoom"));
    zoomResetAct->setStatusTip(tr("Reset zoom"));
    zoomResetAct->setShortcut(QKeySequence("CTRL+0"));
    connect(new QShortcut(QKeySequence("CTRL+0"), this), SIGNAL(activated()),
            zoomResetAct, SLOT(trigger()));
  } connect(zoomOutAct, SIGNAL(triggered()), SLOT(zoomOut()));

  QAction *a;
  if (a = page()->action(QWebPage::OpenLinkInNewWindow)) {
    a->setShortcut(QKeySequence("CTRL"));
  }
  if (a = page()->action(QWebPage::Reload)) {
    a->setShortcut(QKeySequence("CTRL+R"));
    connect(new QShortcut(QKeySequence("CTRL+R"), this), SIGNAL(activated()), a, SLOT(trigger()));
    connect(new QShortcut(QKeySequence("F5"), this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::ReloadAndBypassCache)) {
    a->setShortcut(QKeySequence("CTRL+SHIFT+R"));
    connect(new QShortcut(QKeySequence("CTRL+SHIFT+R"), this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::Forward)) {
    a->setShortcut(QKeySequence::Forward);
    connect(new QShortcut(QKeySequence::Forward, this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::Back)) {
    a->setShortcut(QKeySequence::Back);
    connect(new QShortcut(QKeySequence::Back, this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::Stop)) {
    a->setShortcut(QKeySequence("ESC"));
    connect(new QShortcut(QKeySequence("ESC"), this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::InspectElement)) {
    a->setShortcut(QKeySequence("CTRL+I"));
    connect(new QShortcut(QKeySequence("CTRL+I"), this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::Copy)) {
    //a->setShortcut(QKeySequence::Copy);
    connect(new QShortcut(QKeySequence::Copy, this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::Cut)) {
    //a->setShortcut(QKeySequence::Cut);
    connect(new QShortcut(QKeySequence::Cut, this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::Paste)) {
    //a->setShortcut(QKeySequence::Paste);
    connect(new QShortcut(QKeySequence::Paste, this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::Paste)) {
    //a->setShortcut(QKeySequence::Paste);
    connect(new QShortcut(QKeySequence::Paste, this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::Redo)) {
    //a->setShortcut(QKeySequence::Redo);
    connect(new QShortcut(QKeySequence::Redo, this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::Undo)) {
    //a->setShortcut(QKeySequence::Undo);
    connect(new QShortcut(QKeySequence::Undo, this), SIGNAL(activated()), a, SLOT(trigger()));
  }
  if (a = page()->action(QWebPage::SelectAll)) {
    //a->setShortcut(QKeySequence::SelectAll);
    connect(new QShortcut(QKeySequence::SelectAll, this), SIGNAL(activated()), a, SLOT(trigger()));
  }
}

// - Properties -

QString
QtExt::
WebView::hoveredLink() const
{
  WebPage *p = dynamic_cast<WebPage *>(page());
  return p ? p->hoveredLink() : QString();
}

// - Actions -

QString
QtExt::
WebView::fileNameFromUrl(const QUrl &url, const QString &suffix)
{
  QString fileName; {
    if (url.path().size() > 1) {
      fileName = QFileInfo(url.path()).fileName();
      fileName = QtExt::escapeFileName(fileName);
    }
    if (fileName.isEmpty()) {
      fileName = QtExt::escapeFileName(url.host() + url.path());
      if (fileName.isEmpty())
        fileName = tr("Unnamed");
    }
    fileName = SAVE_PATH + FILE_PATH_SEP + fileName;
  }
  QString ret = fileName;
  if (!suffix.isEmpty() && !ret.endsWith(suffix, Qt::CaseInsensitive))
    ret.append(suffix);

  for (int i = 2; QFile::exists(ret); i++)
    ret = fileName + " " + QString::number(i) + suffix;
  Q_ASSERT(!ret.isEmpty());
  return ret;
}

void
QtExt::
WebView::clip()
{
  // See: http://labs.qt.nokia.com/2008/11/03/thumbnail-preview-of-web-page/
  // See: http://qt.gitorious.org/qt-labs/graphics-dojo/blobs/master/websnap/websnap.cpp
  // find proper size, we stick to sensible aspect ratio

  enum { zoom = 1 };

  QWebFrame *f = page()->mainFrame();
  Q_ASSERT(f);
  // FIXME: how to check if f does not contain error?

  QSize size = f->contentsSize();
  if (size.isEmpty())
    return;

  QUrl url = f->url();
  emit message(tr("clipping") + ": " + url.toString());

  page()->setViewportSize(size);

  QImage image(size, QImage::Format_ARGB32_Premultiplied); {
    image.fill(Qt::transparent);
    QPainter p(&image);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    f->render(&p);
  }

  if (zoom != 1) {
    size *= zoom;
    image = image.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
  }

  QString fileName = fileNameFromUrl(url, ".png");

  QClipboard *c = QApplication::clipboard();
  if (c)
    c->setImage(image);

  bool ok = image.save(fileName, "png");
  if (ok) {
    emit message(tr("saved") + ": " + fileName);
  } else
    emit warning(tr("unable to save") + ": " + fileName);
}

void
QtExt::
WebView::download(const QNetworkRequest &req)
{
#ifdef WITH_MODULE_DOWNLOAD
  enum { retries = 3 };

  QUrl url = req.url();
  QString fileName = fileNameFromUrl(url);

  bool ok = ::dlget(fileName, req, true, retries, this); // async = true
  if (ok)
    emit message(fileName);
  else
    emit warning(tr("failed to download %1").arg(url.toString()));
#else
  Q_UNUSED(req);
  emit warning(tr("download is not allowed"));
#endif // WITH_MODULE_DOWNLOAD
}

// - Events -

QMenu*
QtExt::
WebView::createContextMenu()
{
  QMenu *m = page()->createStandardContextMenu();
  //QAction *a;
  //if (a = page()->action(QWebPage::ReloadAndBypassCache))
  //  m->addAction(a);
  m->addSeparator();
  m->addAction(clipAct);
  m->addSeparator();
  m->addAction(zoomInAct);
  m->addAction(zoomOutAct);
  m->addAction(zoomResetAct);
  return m;
}

void
QtExt::
WebView::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);

  QMenu *m = createContextMenu();
  m->exec(event->globalPos());
  delete m;
  event->accept();
}

void
QtExt::
WebView::wheelEvent(QWheelEvent *event)
{
  Q_ASSERT(event);
  if (event->orientation() == Qt::Vertical &&
      event->modifiers() == Qt::ControlModifier) {
    //qreal z = textSizeMultiplier();
    //z += event->delta() / (8 * 360.0);
    if (event->delta() > 0)
      zoomIn();
    else if (event->delta() < 0)
      zoomOut();
    event->accept();
  } else
    Base::wheelEvent(event);
}

// - Zoom -

void
QtExt::
WebView::zoomIn()
{
  qreal z = zoomFactor();
  z *= 1.2;
  if (z < ZOOM_MAX)
    setZoomFactor(z);
}

void
QtExt::
WebView::zoomOut()
{
  qreal z = zoomFactor();
  z /= 1.2;
  if (z > ZOOM_MIN)
    setZoomFactor(z);
}

void
QtExt::
WebView::zoomReset()
{ setZoomFactor(1.0); }

// - Messages -

void
QtExt::
WebView::showLink(const QString &url)
{ emit message(url.isEmpty() ? url : QUrl::fromPercentEncoding(url.toLocal8Bit())); }

// - State -

void
QtExt::
WebView::setLoading()
{
  DOUT("enter");
  loading_ = true;
  setCursor(Qt::BusyCursor);
  DOUT("exit");
}

void
QtExt::
WebView::setFinished()
{
  DOUT("enter");
  loading_ = false;
  setCursor(Qt::ArrowCursor);
  DOUT("exit");
}

// EOF
