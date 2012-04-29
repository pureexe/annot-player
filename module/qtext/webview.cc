// qtext/webview.cc
// 10/9/2011

#include "module/qtext/webview.h"
#include "module/qtext/actionwithid.h"
#include "module/qtext/algorithm.h"
#include "module/qtext/ss.h"
#include "module/qtext/webpage.h"
#ifdef WITH_MODULE_DOWNLOAD
#  include "module/qtext/filesystem.h"
#  include "module/download/download.h"
#endif // WITH_MODULE_DOWNLOAD
#include <QtWebKit>
#include <QtGui>
#include <QtCore>

//#define DEBUG "qtext::webview"
#include "module/debug/debug.h"

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around assignment
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

#define SAVE_PATH       QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);

//#ifdef Q_WS_MAC
//#  define K_CTRL        "cmd"
//#else
//#  define K_CTRL        "Ctrl"
//#endif // Q_WS_MAC

QString
QtExt::
WebView::shortenText(const QString &text, int len)
{
  Q_ASSERT(len > 3);
  return len && text.size() < len ? text :
         text.left(len - 3).append("...");
}

QtExt::
WebView::WebView(QWidget *parent)
  : Base(parent), loading_(false)
{
  //setWindowFlags(f ? f : WINDOW_FLAGS);

  setTextSizeMultiplier(TEXT_SIZE_SCALE);

  // Antialias and smooth pixel is not turned on to improve performance.
  setRenderHints(QPainter::TextAntialiasing);

  WebPage *page = new WebPage(this); {
    connect(page, SIGNAL(linkHovered(QString,QString,QString)), SLOT(showLink(QString,QString,QString)));
    connect(page, SIGNAL(openLinkRequested(QString)), SIGNAL(openLinkRequested(QString)));
    connect(page, SIGNAL(downloadRequested(QNetworkRequest)), SLOT(download(QNetworkRequest)));

    connect(page, SIGNAL(loadStarted()), SLOT(setLoading()));
    connect(page, SIGNAL(loadFinished(bool)), SLOT(setFinished()));

    //connect(page, SIGNAL(loadStarted()), SLOT(resetStyleSheet()), Qt::QueuedConnection);
    //connect(this, SIGNAL(loadStarted()), SLOT(setLoading()));
    //connect(this, SIGNAL(loadFinished(bool)), SLOT(setFinished()));
    connect(page, SIGNAL(selectionChanged()), SIGNAL(selectionChanged()));
    connect(page, SIGNAL(linkClicked(QUrl)), SIGNAL(linkClicked(QUrl)));
    //connect(page, SIGNAL(scrollRequested(int,int,QRect)), SLOT(scroll(int,int,QRect)));
  } setPage(page);

  createActions();

  connect(this, SIGNAL(selectionChanged()), SLOT(invalidteSelection()));

  //setStyleSheet(SS_WEBVIEW);
}

void
QtExt::
WebView::createActions()
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
WebView::openWithOperatingSystem()
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
    emit message(tr("openning") + ": " + u.toString());
    QDesktopServices::openUrl(u);
  }
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

// - Menu -

QMenu*
QtExt::
WebView::createHistoryMenu()
{
  QWebHistory *h = history();
  if (!h || h->count() <= 1)
    return 0;

  QList<QWebHistoryItem> items = h->items();

  QMenu *m = new QMenu(tr("History"), this);
  int i = 0;
  QList<QAction *> actions;
  foreach (const QWebHistoryItem &item, items) {
    ActionWithId *a = new ActionWithId(i++, m);
    a->setCheckable(true);
    a->setChecked(item.url() == url());
    QUrl u = item.originalUrl();
    if (u.isEmpty())
      u = item.url();
    QString text;
    if (!u.isEmpty())
      text = u.toString(QUrl::RemoveAuthority | QUrl::RemoveFragment | QUrl::StripTrailingSlash);
    else
      text = tr("Blank");
    text.prepend(QString::number(i) + ". ");
    a->setText(text);
    a->setIcon(item.icon());
    connect(a, SIGNAL(triggeredWithId(int)), SLOT(goToRecent(int)));
    actions.append(a);
  }
  m->addActions(revertList(actions));
  return m;
}

QMenu*
QtExt::
WebView::createContextMenu()
{
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

  if (history)
    QTimer::singleShot(0, history, SLOT(deleteLater()));
  return m;
}

// - Events -

void
QtExt::
WebView::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);
  updateHoveredLink();
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

void
QtExt::
WebView::clearHighlight()
{ findText(QString(), QWebPage::HighlightAllOccurrences); }

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

void
QtExt::
WebView::goToRecent(int i)
{
  QWebHistory *h = history();
  if (h && h->count() > i)
    h->goToItem(h->itemAt(i));
}

// - Messages -

void
QtExt::
WebView::showLink(const QString &url, const QString &title, const QString &content)
{
  if (url.isEmpty())
    emit message(QString());
  else {
    QString m = QUrl::fromPercentEncoding(url.toUtf8());
    QString t = content.isEmpty() ? title : content;
    t = t.simplified();
    if (!t.isEmpty())
      m.prepend(shortenText(t) + " | ");
    emit message(m);
  }
}

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

void
QtExt::
WebView::invalidteSelection()
{
  enum { min = 3, max = 100 };
  QString s = selectedText().trimmed();
  if (s.size() >= min && s.size() <= max) {
    clearHighlight();
    findText(s, QWebPage::FindWrapsAroundDocument | QWebPage::HighlightAllOccurrences);
  }
}

void QtExt::WebView::scrollTop()    { WebPage *p = dynamic_cast<WebPage *>(page()); if (p) p->scrollTop(); }
void QtExt::WebView::scrollBottom() { WebPage *p = dynamic_cast<WebPage *>(page()); if (p) p->scrollBottom(); }
void QtExt::WebView::scrollLeft()   { WebPage *p = dynamic_cast<WebPage *>(page()); if (p) p->scrollLeft(); }
void QtExt::WebView::scrollRight()  { WebPage *p = dynamic_cast<WebPage *>(page()); if (p) p->scrollRight(); }

// EOF
