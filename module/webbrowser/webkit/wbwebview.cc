// wbwebview.cc
// 1/27/2012
#include "webkit/wbwebview.h"
#include "global/wbrc.h"
#include "project/common/acrc.h"
#include "module/searchengine/searchenginefactory.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include "module/qtext/actionwithid.h"
#include <QtGui>

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around assignment
#endif // __GNUC__

#ifdef Q_WS_MAC
#  define K_META        "META"
#  define K_CAPSLOCK    "capslock"
#else
#  define K_META        "ALT"
#  define K_CAPSLOCK    "CapsLock"
#endif // Q_WS_MAC

// - Construction -

WbWebView::WbWebView(QWidget *parent)
  : Base(parent), contextMenu_(0), searchEngine_(0)
{
  createActions();
  connect(this, SIGNAL(selectionChanged()), SLOT(invalidateSelection()));
  connect(this, SIGNAL(loadFinished(bool)), SLOT(updateTitle()));
  connect(page(), SIGNAL(linkHovered(QString,QString,QString)), SLOT(highlightDownloadableLink(QString,QString,QString)));
}

WbWebView::~WbWebView()
{
  if (!deleteLater_.isEmpty())
    foreach (QObject *p, deleteLater_)
      p->deleteLater();
}

void
WbWebView::createActions()
{
  openSelectedLinkAct_ = new QAction(this); {
    openSelectedLinkAct_->setText(tr("Open Link in New Tab"));
    openSelectedLinkAct_->setStatusTip(tr("Open Link in New Tab"));
  } connect(openSelectedLinkAct_, SIGNAL(triggered()), SLOT(openSelectedLink()));
  openWithAcPlayerAct_ = new QAction(this); {
    openWithAcPlayerAct_->setIcon(QIcon(ACRC_IMAGE_PLAYER));
    openWithAcPlayerAct_->setText(tr("Play with Annot Player"));
    openWithAcPlayerAct_->setStatusTip(tr("Play with Annot Player"));
  } connect(openWithAcPlayerAct_, SIGNAL(triggered()), SLOT(openWithAcPlayer()));
  importToAcPlayerAct_ = new QAction(this); {
    importToAcPlayerAct_->setIcon(QIcon(WBRC_IMAGE_COMMENT));
    importToAcPlayerAct_->setText(tr("Import annotations to Annot Player"));
    importToAcPlayerAct_->setStatusTip(tr("Import annotations to Annot Player"));
  } connect(importToAcPlayerAct_, SIGNAL(triggered()), SLOT(importToAcPlayer()));
  openWithAcDownloaderAct_ = new QAction(this); {
    openWithAcDownloaderAct_->setIcon(QIcon(ACRC_IMAGE_DOWNLOADER));
    openWithAcDownloaderAct_->setText(tr("Download with Annot Downloader"));
    openWithAcDownloaderAct_->setStatusTip(tr("Download with Annot Downloader"));
  } connect(openWithAcDownloaderAct_, SIGNAL(triggered()), SLOT(openWithAcDownloader()));
  undoClosedTabAct_ = new QAction(this); {
    undoClosedTabAct_->setText(tr("Undo Close Tab"));
    undoClosedTabAct_->setStatusTip(tr("Undo Close Tab"));
    undoClosedTabAct_->setShortcut(QKeySequence("CTRL+SHIFT+T"));
  } connect(undoClosedTabAct_, SIGNAL(triggered()), SIGNAL(undoClosedTabRequested()));
  newWindowAct_ = new QAction(this); {
    newWindowAct_->setText(tr("New Window"));
    newWindowAct_->setStatusTip(tr("New Window"));
    newWindowAct_->setShortcut(QKeySequence::New);
  } connect(newWindowAct_, SIGNAL(triggered()), SIGNAL(newWindowRequested()));
  fullScreenAct_ = new QAction(this); {
    fullScreenAct_->setText(tr("Toggle Full Screen"));
    fullScreenAct_->setStatusTip(tr("Toggle Full Screen"));
    fullScreenAct_->setShortcut(QKeySequence("CTRL+" K_META "+F"));
  } connect(fullScreenAct_, SIGNAL(triggered()), SIGNAL(fullScreenRequested()));
  menuBarAct_ = new QAction(this); {
    menuBarAct_->setText(tr("Toggle Menu Bar") + " [" K_CAPSLOCK "]");
    menuBarAct_->setStatusTip(tr("Toggle Menu Bar"));
  } connect(menuBarAct_, SIGNAL(triggered()), SIGNAL(toggleMenuBarVisibleRequested()));
}

// - Events -

QWebView*
WbWebView::createWindow(QWebPage::WebWindowType type)
{
  if (type != QWebPage::WebBrowserWindow)
    return Base::createWindow(type);
  Self *ret = new Self(parentWidget());
  emit windowCreated(ret);
  return ret;
}

void
WbWebView::contextMenuEvent(QContextMenuEvent *event)
{
  updateHoveredLink();
  Q_ASSERT(event);
  if (!contextMenu_)
    contextMenu_ = new QMenu(this);
  contextMenu_->clear();
  if (!deleteLater_.isEmpty()) {
    foreach (QObject *p, deleteLater_)
      p->deleteLater();
    deleteLater_.clear();
  }

  currentUrl_ = hoveredLink();
  QString selectedLink = selectedUrl();
  if (currentUrl_.isEmpty()) {
    currentUrl_ = selectedLink;
    if (currentUrl_.isEmpty())
      currentUrl_ = url().toString();
  }
  int site = MrlAnalysis::matchSite(currentUrl_);
  if (site) {
    contextMenu_->addAction(openWithAcPlayerAct_);
    if (site < MrlAnalysis::AnnotationSite)
      contextMenu_->addAction(importToAcPlayerAct_);
    if (site < MrlAnalysis::ChineseVideoSite) // TODO: change to all sites after fixing youtube
      contextMenu_->addAction(openWithAcDownloaderAct_);
    contextMenu_->addSeparator();
  }
  if (!selectedLink.isEmpty()) {
    contextMenu_->addAction(openSelectedLinkAct_);
    contextMenu_->addSeparator();
  }

  QMenu *scm = page()->createStandardContextMenu();
  deleteLater_.append(scm);

  QString selection = selectedText().simplified();
  if (!selection.isEmpty() && hasSearchEngines()) {
    for (int engine = 0; engine < searchEngines_.size(); engine++) {
      QtExt::ActionWithId *a = new QtExt::ActionWithId(engine);
      deleteLater_.append(a);
      SearchEngine *e = searchEngines_[engine];
      a->setText(tr("Search with %1").arg(e->name()));
      a->setStatusTip(e->search("@key"));
      a->setIcon(QIcon(e->icon()));
      a->setCheckable(true);
      if (engine == searchEngine_)
        a->setChecked(true);
      connect(a, SIGNAL(triggeredWithId(int,bool)), SLOT(searchWithEngine(int)));
      contextMenu_->addAction(a);
    }

    int engine = SearchEngineFactory::Qt;
    if (engine >= searchEngines_.size() && selection.startsWith('Q') && !selection.contains(' ')) {
      QtExt::ActionWithId *a = new QtExt::ActionWithId(engine);
      deleteLater_.append(a);
      static SearchEngine *e = 0;
      if (!e)
        e = SearchEngineFactory::globalInstance()->create(engine);
      a->setText(tr("Search with %1").arg(e->name()));
      a->setStatusTip(e->search("@key"));
      a->setIcon(QIcon(e->icon()));
      a->setCheckable(true);
      if (engine == searchEngine_)
        a->setChecked(true);
      connect(a, SIGNAL(triggeredWithId(int,bool)), SLOT(searchWithEngine(int)));
      contextMenu_->addAction(a);
    }

    contextMenu_->addSeparator();
  }

  contextMenu_->addActions(contextMenu_->actions());
  //QAction *a;
  //if (a = page()->action(QWebPage::ReloadAndBypassCache))
  //  contextMenu_->addAction(a);

  contextMenu_->addSeparator();
  QMenu *history = createHistoryMenu();
  if (history) {
    deleteLater_.append(history);
    contextMenu_->addMenu(history);
    contextMenu_->addSeparator();
  }
  contextMenu_->addSeparator();
  contextMenu_->addAction(clipAct);
  contextMenu_->addAction(undoClosedTabAct_);
  contextMenu_->addAction(clearHighlightAct);
  contextMenu_->addSeparator();
  contextMenu_->addAction(zoomResetAct);
  contextMenu_->addAction(zoomInAct);
  contextMenu_->addAction(zoomOutAct);
  contextMenu_->addSeparator();
  contextMenu_->addAction(openWithOperatingSystemAct);
  contextMenu_->addAction(newWindowAct_);
  contextMenu_->addAction(fullScreenAct_);
#ifdef Q_WS_WIN
  contextMenu_->addAction(menuBarAct_);
#endif // Q_WS_WIN

  contextMenu_->exec(event->globalPos());
  event->accept();
}

void
WbWebView::invalidateSelection()
{
  QString selection = selectedUrl();
  if (!selection.isEmpty())
    emit message(selection);
}

QString
WbWebView::selectedUrl() const
{
  QString ret = selectedText().trimmed();
  if (ret.isEmpty())
    return ret;
  ret = ret.split('\n', QString::SkipEmptyParts).first();
  if (ret.contains(QRegExp("[a-zA-Z]{3,5}://[a-zA-Z]")))
    return ret;
  else if (ret.startsWith("www."))
    return ret.prepend("http://");
  else if (ret.startsWith("ttp://"))
    return ret.prepend('h');
  else
    return QString();
}

// - Properties -

void
WbWebView::updateTitle()
{
  QString t = title().trimmed();
  if (!t.isEmpty()) {
    QTime now = QTime::currentTime();
    QString ts = now.toString(" | h:mm");
    t.append(ts);
    emit titleChanged(t);
  }
}

// - Search -

void
WbWebView::searchWithEngine(int engine)
{
  QString t = selectedText().simplified();
  if (!t.isEmpty())
    emit searchWithEngineRequested(t, engine);
}

// - Download -

void
WbWebView::highlightDownloadableLink(const QString &link, const QString &title, const QString &content)
{
  enum { min = 2, max = 100 };
  if (!link.isEmpty()) {
    QString t = content.isEmpty() ? title : content;
    t = t.trimmed();
    if (t.size() >= min && t.size() <= 100 && MrlAnalysis::matchSite(link))
      page()->findText(t,
                     QWebPage::FindCaseSensitively | QWebPage::FindWrapsAroundDocument | QWebPage::HighlightAllOccurrences);
  }
}

void
WbWebView::showLink(const QString &url, const QString &title, const QString &content)
{
  if (url.isEmpty())
    emit message(QString());
  else {
    QString m = QUrl::fromPercentEncoding(url.toUtf8());
    QString t = content.isEmpty() ? title : content;
    t = t.simplified();
    if (!t.isEmpty())
      m.prepend(shortenText(t) + " | ");
    if (MrlAnalysis::matchSite(url))
      emit notification(m);
    else
      emit message(m);
  }
}

// EOF
