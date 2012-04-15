// qtext/webdialog.cc
// 10/9/2011

#include "module/qtext/webdialog.h"
#include "module/qtext/webpage.h"
#ifdef WITH_MODULE_DOWNLOAD
#  include "module/qtext/filesystem.h"
#  include "module/download/download.h"
#endif // WITH_MODULE_DOWNLOAD
#include <QtGui>
#include <QtWebKit>

#define WINDOW_FLAGS \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint

#define TEXT_SIZE_SCALE 0.85
#define ZOOM_MAX        5.0
#define ZOOM_MIN        0.5

//#ifdef Q_WS_MAC
//#  define K_CTRL        "cmd"
//#else
//#  define K_CTRL        "Ctrl"
//#endif // Q_WS_MAC

QtExt::
WebDialog::WebDialog(QWidget *parent, Qt::WindowFlags f)
  : Base(parent)
{
  setWindowFlags(f ? f : WINDOW_FLAGS);

  setTextSizeMultiplier(TEXT_SIZE_SCALE);

  QWebPage *page = new WebPage(this); {
    connect(page, SIGNAL(linkHovered(QString,QString,QString)), SIGNAL(message(QString)));
    connect(page, SIGNAL(downloadRequested(QNetworkRequest)), SLOT(download(QNetworkRequest)));
  } setPage(page);

  createActions();
}

void
QtExt::
WebDialog::createActions()
{
  zoomInAct = new QAction(this); {
    zoomInAct->setText(tr("Zoom in"));
    zoomInAct->setStatusTip(tr("Zoom in"));
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
  } connect(zoomInAct, SIGNAL(triggered()), SLOT(zoomIn()));

  zoomOutAct = new QAction(this); {
    zoomOutAct->setText(tr("Zoom out"));
    zoomOutAct->setStatusTip(tr("Zoom out"));
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
  } connect(zoomOutAct, SIGNAL(triggered()), SLOT(zoomOut()));

  zoomResetAct = new QAction(this); {
    zoomResetAct->setText(tr("Reset zoom"));
    zoomResetAct->setStatusTip(tr("Reset zoom"));
    zoomResetAct->setShortcut(QKeySequence("CTRL+0"));
  } connect(zoomOutAct, SIGNAL(triggered()), SLOT(zoomOut()));

  QAction *a = page()->action(QWebPage::Reload);
  if (a)
    a->setShortcut(QKeySequence("CTRL+R"));
  a = page()->action(QWebPage::Forward);
  if (a)
    a->setShortcut(QKeySequence::Forward);
  a = page()->action(QWebPage::Back);
  if (a)
    a->setShortcut(QKeySequence::Back);
  a = page()->action(QWebPage::Stop);
  if (a)
    a->setShortcut(QKeySequence("ESC"));

  QShortcut *zoomInShortcut = new QShortcut(QKeySequence("CTRL+="), this);
  connect(zoomInShortcut, SIGNAL(activated()), SLOT(zoomIn()));
  QShortcut *zoomOutShortcut = new QShortcut(QKeySequence("CTRL+-"), this);
  connect(zoomOutShortcut, SIGNAL(activated()), SLOT(zoomOut()));
  QShortcut *zoomResetShortcut = new QShortcut(QKeySequence("CTRL+0"), this);
  connect(zoomResetShortcut, SIGNAL(activated()), SLOT(zoomReset()));
  QShortcut *refreshShortcut = new QShortcut(QKeySequence("CTRL+R"), this);
  connect(refreshShortcut, SIGNAL(activated()), SLOT(reload()));
  QShortcut *forwardShortcut = new QShortcut(QKeySequence::Forward, this);
  connect(forwardShortcut, SIGNAL(activated()), SLOT(forward()));
  QShortcut *backShortcut = new QShortcut(QKeySequence::Back, this);
  connect(backShortcut, SIGNAL(activated()), SLOT(back()));
  QShortcut *stopShortcut = new QShortcut(QKeySequence("ESC"), this);
  connect(stopShortcut, SIGNAL(activated()), SLOT(stop()));
}

// - Properties -

QString
QtExt::
WebDialog::hoveredLink() const
{
  WebPage *p = dynamic_cast<WebPage *>(page());
  return p ? p->hoveredLink() : QString::null;
}

// - Download -

void
QtExt::
WebDialog::download(const QNetworkRequest &req)
{
#ifdef WITH_MODULE_DOWNLOAD
  enum { retries = 3 };

  QString path = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  QString fileName = QFileInfo(req.url().path()).fileName();
  fileName = QtExt::escapeFileName(fileName);

  path += FILE_PATH_SEP + fileName;
  bool ok = ::dlget(path, req, true, retries, this); // async = true
  if (ok)
    emit message(path);
  else
    emit warning(tr("failed to download %1").arg(req.url().toString()));
  //QApplication::beep();
#else
  Q_UNUSED(req);
  emit warning(tr("download is not allowed"));
#endif // WITH_MODULE_DOWNLOAD
}

// - Events -

QMenu*
QtExt::
WebDialog::createContextMenu()
{
  QMenu *m = page()->createStandardContextMenu();
  m->addSeparator();
  m->addAction(zoomInAct);
  m->addAction(zoomOutAct);
  m->addAction(zoomResetAct);
  return m;
}

void
QtExt::
WebDialog::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);

  QMenu *m = createContextMenu();
  m->exec(event->globalPos());
  delete m;
  event->accept();
}

void
QtExt::
WebDialog::wheelEvent(QWheelEvent *event)
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
WebDialog::zoomIn()
{
  qreal z = zoomFactor();
  z *= 1.2;
  if (z < ZOOM_MAX)
    setZoomFactor(z);
}

void
QtExt::
WebDialog::zoomOut()
{
  qreal z = zoomFactor();
  z /= 1.2;
  if (z > ZOOM_MIN)
    setZoomFactor(z);
}

void
QtExt::
WebDialog::zoomReset()
{ setZoomFactor(1.0); }

// EOF
