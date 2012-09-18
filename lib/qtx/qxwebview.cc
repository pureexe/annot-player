// qxwebview.cc
// 10/9/2011

#include "qtx/qxwebview.h"
#include "qtx/qxactionwithid.h"
#include "qtx/qxfs.h"
#include "qtx/qxwebpage.h"
#include "htmlutil/sstags.h"
#ifdef WITH_LIB_DOWNLOAD
# include "lib/download/download.h"
#endif // WITH_LIB_DOWNLOAD
#include <QtGui>
#include <QtWebKit>

//#define DEBUG "qtext::webview"
#include "qtx/qxdebug.h"

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around assignment
#endif // __GNUC__

#define SS_WEBVIEW \
  SS_BEGIN(QWebView) \
    SS_BACKGROUND_COLOR(#ececec) \
  SS_END

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

#define SAVE_PATH       QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)

//#ifdef Q_OS_MAC
//# define K_CTRL        "cmd"
//#else
//# define K_CTRL        "Ctrl"
//#endif // Q_OS_MAC

QTX_BEGIN_NAMESPACE

QString
QxWebView::shortenText(const QString &text, int len)
{
  Q_ASSERT(len > 3);
  return len && text.size() < len ? text :
         text.left(len - 3).append("...");
}

QxWebView::QxWebView(QWidget *parent)
  : Base(parent), progress_(100)
{
  //setWindowFlags(f ? f : WINDOW_FLAGS);

  setTextSizeMultiplier(TEXT_SIZE_SCALE);

  // Antialias and smooth pixel is not turned on to improve performance.
  setRenderHints(QPainter::TextAntialiasing);

  //connect(this, SIGNAL(linkClicked(QUrl)), SLOT(setLoading()));

  QxWebPage *page = new QxWebPage(this); {
    connect(page, SIGNAL(linkHovered(QString,QString,QString)), SLOT(showLink(QString,QString,QString)));
    connect(page, SIGNAL(openLinkRequested(QString)), SIGNAL(openLinkRequested(QString)));
    connect(page, SIGNAL(downloadRequested(QNetworkRequest)), SLOT(download(QNetworkRequest)));
    connect(page, SIGNAL(menuBarVisibilityChangeRequested(bool)), SIGNAL(menuBarVisibilityChangeRequested(bool)));

    connect(page, SIGNAL(loadProgress(int)), SLOT(updateProgress(int)));
    connect(page, SIGNAL(loadStarted()), SLOT(setLoading()));
    connect(page, SIGNAL(loadFinished(bool)), SLOT(setFinished()));

    //connect(page, SIGNAL(loadStarted()), SLOT(resetStyleSheet()), Qt::QueuedConnection);
    //connect(this, SIGNAL(loadStarted()), SLOT(setLoading()));
    //connect(this, SIGNAL(loadFinished(bool)), SLOT(setFinished()));
    connect(page, SIGNAL(selectionChanged()), SIGNAL(selectionChanged()));
    //connect(page, SIGNAL(linkClicked(QUrl)), SIGNAL(linkClicked(QUrl))); // links are not delegated
    //connect(page, SIGNAL(scrollRequested(int,int,QRect)), SLOT(scroll(int,int,QRect)));
  } setPage(page);

  createActions();

  connect(this, SIGNAL(selectionChanged()), SLOT(invalidteSelection()));

  //grabGesture(Qt::PanGesture);
  grabGesture(Qt::SwipeGesture);
  grabGesture(Qt::PinchGesture);

  //setStyleSheet(SS_WEBVIEW);
}

void
QxWebView::createActions()
{
  clipAct = new QAction(this); {
    clipAct->setText(tr("Clip the Page"));
    clipAct->setStatusTip(tr("Save the web page as image to disk and clipboard"));
    clipAct->setShortcut(QKeySequence::Save);
    connect(new QShortcut(QKeySequence::Save, this), SIGNAL(activated()),
            clipAct, SLOT(trigger()));
  } connect(clipAct, SIGNAL(triggered()), SLOT(clip()));

  zoomInAct = new QAction(this); {
    zoomInAct->setText(tr("Zoom In"));
    zoomInAct->setStatusTip(tr("Zoom In"));
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    connect(new QShortcut(QKeySequence("CTRL+="), this), SIGNAL(activated()),
            zoomInAct, SLOT(trigger()));
  } connect(zoomInAct, SIGNAL(triggered()), SLOT(zoomIn()));

  zoomOutAct = new QAction(this); {
    zoomOutAct->setText(tr("Zoom Out"));
    zoomOutAct->setStatusTip(tr("Zoom Out"));
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    connect(new QShortcut(QKeySequence("CTRL+-"), this), SIGNAL(activated()),
            zoomOutAct, SLOT(trigger()));
  } connect(zoomOutAct, SIGNAL(triggered()), SLOT(zoomOut()));

  zoomResetAct = new QAction(this); {
    zoomResetAct->setText(tr("Actual Size"));
    zoomResetAct->setStatusTip(tr("Actual Size"));
    zoomResetAct->setShortcut(QKeySequence("CTRL+0"));
    connect(new QShortcut(QKeySequence("CTRL+0"), this), SIGNAL(activated()),
            zoomResetAct, SLOT(trigger()));
  } connect(zoomOutAct, SIGNAL(triggered()), SLOT(zoomOut()));

  clearHighlightAct = new QAction(this); {
    clearHighlightAct->setText(tr("Clear Highlight"));
    clearHighlightAct->setStatusTip(tr("Clear Highlight"));
  } connect(clearHighlightAct, SIGNAL(triggered()), SLOT(clearHighlight()));

  openWithOperatingSystemAct = new QAction(this); {
    openWithOperatingSystemAct->setText(tr("Open in System Default Browser"));
    openWithOperatingSystemAct->setStatusTip(tr("Open in System Default Browser"));
  } connect(openWithOperatingSystemAct, SIGNAL(triggered()), SLOT(openWithOperatingSystem()));

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
    a->setShortcut(QKeySequence("CTRL+."));
    connect(new QShortcut(QKeySequence("CTRL+."), this), SIGNAL(activated()), a, SLOT(trigger()));
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
QxWebView::hoveredLink() const
{
  auto p = qobject_cast<QxWebPage *>(page());
  return p ? p->hoveredLink() : QString();
}

// - Actions -

void
QxWebView::clearHighlight()
{ findText(QString(), QWebPage::HighlightAllOccurrences); }

QString
QxWebView::fileNameFromUrl(const QUrl &url, const QString &suffix)
{
  QString fileName; {
    if (url.path().size() > 1) {
      fileName = QFileInfo(url.path()).fileName();
      fileName = qxEscapeFileName(fileName);
    }
    if (fileName.isEmpty()) {
      fileName = qxEscapeFileName(url.toString(QUrl::RemoveScheme | QUrl::RemovePort | QUrl::RemoveQuery | QUrl::StripTrailingSlash));
      fileName = QFileInfo(fileName).fileName();
      if (fileName.isEmpty())
        fileName = tr("Unnamed");
    }
    fileName.prepend(SAVE_PATH + QDir::separator());
  }
  QString suf = suffix;
  if (suf.isEmpty()) {
    suf = QFileInfo(fileName).completeSuffix();
    if (!suf.isEmpty()) {
      suf.prepend('.');
      fileName = fileName.left(fileName.size() - suf.size());
    }
  } else if (fileName.endsWith(suf, Qt::CaseInsensitive))
    fileName = fileName.left(fileName.size() - suf.size());

  QString ret = fileName + suf;

  for (int i = 2; QFile::exists(ret); i++)
    ret = fileName + " " + QString::number(i) + suf;
  Q_ASSERT(!ret.isEmpty());
  return ret;
}

void
QxWebView::openWithOperatingSystem()
{
  QString address = hoveredLink_;
  if (address.isEmpty()) {
    address = selectedText().trimmed();
    if (!address.isEmpty()) {
      if (address.startsWith("ttp://"))
        address.prepend('h');
      else if (!address.startsWith("http"))
        address.clear();
    }
  }

  QUrl u = address.isEmpty() ? url() : QUrl(address);
  if (!u.isEmpty()) {
    emit message(tr("opening") + ": " + u.toString());
    QDesktopServices::openUrl(u);
  }
}

void
QxWebView::clip()
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
QxWebView::download(const QNetworkRequest &req)
{
#ifdef WITH_LIB_DOWNLOAD
  enum { retries = 3 };
  DOUT("enter: url =" << req.url().toString());

  QUrl url = req.url();
  QString fileName = fileNameFromUrl(url);

  DOUT("fileName =" << fileName);

  bool ok = ::dlget(fileName, req, true, retries, this); // async = true
  if (ok)
    emit message(fileName);
  else
    emit warning(tr("failed to download %1").arg(url.toString()));
  DOUT("exit: ok =" << ok);
#else
  Q_UNUSED(req)
  emit warning(tr("download is not allowed"));
#endif // WITH_LIB_DOWNLOAD
}

// - Menu -

QMenu*
QxWebView::createHistoryMenu()
{
  DOUT("enter");
  QWebHistory *h = history();
  if (!h || h->count() <= 1) {
    DOUT("exit: no history");
    return 0;
  }

  QList<QWebHistoryItem> items = h->items();

  QMenu *m = new QMenu(tr("History"));
  int i = 0;
  QList<QAction *> actions;
  foreach (const QWebHistoryItem &item, items) {
    if (!item.isValid())
      continue;
    QxActionWithId *a = new QxActionWithId(i++, m);
    a->setCheckable(true);
    a->setChecked(item.url() == url());
    QUrl u = item.originalUrl();
    if (u.isEmpty())
      u = item.url();
    QString text;
    if (!u.isEmpty()) {
      text = u.toString(QUrl::RemoveUserInfo | QUrl::StripTrailingSlash);
      text.remove(QRegExp("^http://"));
    } else
      text = tr("Blank");
    a->setStatusTip(text);
    text.prepend(QString::number(i) + ". ");
    a->setText(shortenText(text));
    //a->setIcon(item.icon()); // FIXME: Randomly CRASH on Qt 4.7.4 and Qt 4.8.1
    connect(a, SIGNAL(triggeredWithId(int)), SLOT(goToRecent(int)));
    actions.append(a);
  }
  if (actions.size() <= 1) {
    delete m;
    return 0;
  }
  //m->addActions(qxRevertList(actions));
  for (auto p = actions.constEnd(); p != actions.constBegin();
       m->addAction(*--p));
  DOUT("exit");
  return m;
}

void
QxWebView::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);
  updateHoveredLink();

  QMenu *m = page()->createStandardContextMenu();
  //QAction *a;
  //if (a = page()->action(QWebPage::ReloadAndBypassCache))
  //  m->addAction(a);
  m->addSeparator();
  QMenu *history = createHistoryMenu();
  if (history) {
    m->addMenu(history);
    m->addSeparator();
  }
  m->addAction(clipAct);
  m->addAction(clearHighlightAct);
  m->addSeparator();
  m->addAction(zoomResetAct);
  m->addAction(zoomInAct);
  m->addAction(zoomOutAct);
  m->addSeparator();
  m->addAction(openWithOperatingSystemAct);

  m->exec(event->globalPos());
  delete m;
  if (history)
    delete history;
  event->accept();
}

// - Events -

bool
QxWebView::event(QEvent *e)
{
  bool accept = true;
  switch (e->type()) {
    // See: examples/gestures/imagegestures
  case QEvent::Gesture: gestureEvent(static_cast<QGestureEvent *>(e)); break;
  default:              accept = Base::event(e);
  }
  return accept;
}

void
QxWebView::gestureEvent(QGestureEvent *e)
{
  DOUT("enter");
  if (QGesture *swipe = e->gesture(Qt::SwipeGesture))
    swipeGesture(static_cast<QSwipeGesture *>(swipe));
  else if (QGesture *pan = e->gesture(Qt::PanGesture))
    panGesture(static_cast<QPanGesture *>(pan));
  else if (QGesture *pinch = e->gesture(Qt::PinchGesture))
    pinchGesture(static_cast<QPinchGesture *>(pinch));
  DOUT("exit");
}

void
QxWebView::wheelEvent(QWheelEvent *event)
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

// - Gestures -

void
QxWebView::panGesture(QPanGesture *g)
{
  DOUT("enter");
  Q_UNUSED(g)
  //switch (gesture->state()) {
  //case Qt::GestureStarted:
  //case Qt::GestureUpdated:
  //  setCursor(Qt::SizeAllCursor);
  //  break;
  //default:
  //  setCursor(Qt::ArrowCursor);
  //}
  //QPointF delta = gesture->delta();
  //horizontalOffset += delta.x();
  //verticalOffset += delta.y();
  DOUT("exit");
}

void
QxWebView::pinchGesture(QPinchGesture *g)
{
  DOUT("enter");
  QPinchGesture::ChangeFlags f = g->changeFlags();
  if (f & QPinchGesture::ScaleFactorChanged) {
    if (g->scaleFactor() > 1)
      zoomIn();
    else
      zoomOut();
  }
  DOUT("exit");
}

void
QxWebView::swipeGesture(QSwipeGesture *g)
{
  DOUT("enter");
  if (g->state() == Qt::GestureFinished) {
    uint h = g->horizontalDirection(),
         d = g->verticalDirection();
    if (h ^ d)
      switch (h | d) {
      case QSwipeGesture::Left:   back(); break;
      case QSwipeGesture::Right:  forward(); break;
      case QSwipeGesture::Up:     scrollTop(); break;
      case QSwipeGesture::Down:   scrollBottom(); break;
      default: Q_ASSERT(0);
      }
  }
  DOUT("exit");
}

// - Zoom -

void
QxWebView::zoomIn()
{
  qreal z = zoomFactor();
  z *= 1.2;
  if (z < ZOOM_MAX)
    setZoomFactor(z);
}

void
QxWebView::zoomOut()
{
  qreal z = zoomFactor();
  z /= 1.2;
  if (z > ZOOM_MIN)
    setZoomFactor(z);
}

void
QxWebView::zoomReset()
{ setZoomFactor(1.0); }

void
QxWebView::goToRecent(int i)
{
  QWebHistory *h = history();
  if (h && h->count() > i)
    h->goToItem(h->itemAt(i));
}

// - Messages -

void
QxWebView::showLink(const QString &url, const QString &title, const QString &content)
{
  if (url.isEmpty())
    emit message(QString());
  else {
    QString m = QUrl::fromPercentEncoding(url.toUtf8());
    m.replace(CONFIG_ANNOT_IP, "annot.me");
    QString t = content.isEmpty() ? title : content;
    t = t.simplified();
    if (!t.isEmpty())
      m.prepend(shortenText(t) + " | ");
    emit message(m);
  }
}

// - State -

void
QxWebView::setLoading()
{
  //DOUT("enter");
  progress_ = 0;
  setCursor(Qt::BusyCursor);
  //DOUT("exit");
}

void
QxWebView::setFinished()
{
  //DOUT("enter");
  progress_ = 100;
  setCursor(Qt::ArrowCursor);
  //DOUT("exit");
}

void
QxWebView::invalidteSelection()
{
  enum { min = 3, max = 100 };
  QString s = selectedText().trimmed();
  if (s.size() >= min && s.size() <= max) {
    clearHighlight();
    findText(s, QWebPage::FindWrapsAroundDocument | QWebPage::HighlightAllOccurrences);
  }
}

void QxWebView::scrollTop()    { auto p = qobject_cast<QxWebPage *>(page()); if (p) p->scrollTop(); }
void QxWebView::scrollBottom() { auto p = qobject_cast<QxWebPage *>(page()); if (p) p->scrollBottom(); }
void QxWebView::scrollLeft()   { auto p = qobject_cast<QxWebPage *>(page()); if (p) p->scrollLeft(); }
void QxWebView::scrollRight()  { auto p = qobject_cast<QxWebPage *>(page()); if (p) p->scrollRight(); }

QTX_END_NAMESPACE
