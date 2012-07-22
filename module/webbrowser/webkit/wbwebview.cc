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
# pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around assignment
#endif // __GNUC__

#ifdef Q_OS_MAC
# define K_META        "META"
# define K_CAPSLOCK    "capslock"
#else
# define K_META        "ALT"
# define K_CAPSLOCK    "CapsLock"
#endif // Q_OS_MAC

// - Construction -

WbWebView::WbWebView(QWidget *parent)
  : Base(parent), searchEngine_(0)
{
  createActions();
  connect(this, SIGNAL(selectionChanged()), SLOT(invalidateSelection()));
  connect(this, SIGNAL(loadFinished(bool)), SLOT(updateTitle()));
  connect(page(), SIGNAL(linkHovered(QString,QString,QString)), SLOT(highlightDownloadableLink(QString,QString,QString)));
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
    importToAcPlayerAct_->setText(tr("Import Annotations to Annot Player"));
    importToAcPlayerAct_->setStatusTip(tr("Import Annotations to Annot Player"));
  } connect(importToAcPlayerAct_, SIGNAL(triggered()), SLOT(importToAcPlayer()));
  openWithAcDownloaderAct_ = new QAction(this); {
    openWithAcDownloaderAct_->setIcon(QIcon(ACRC_IMAGE_DOWNLOADER));
    openWithAcDownloaderAct_->setText(tr("Download with Annot Downloader"));
    openWithAcDownloaderAct_->setStatusTip(tr("Download with Annot Downloader"));
  } connect(openWithAcDownloaderAct_, SIGNAL(triggered()), SLOT(openWithAcDownloader()));
  downloadAnnotationsAct_ = new QAction(this); {
    downloadAnnotationsAct_->setIcon(QIcon(ACRC_IMAGE_BROWSER));
    downloadAnnotationsAct_->setText(tr("Save Annotations to Desktop"));
    downloadAnnotationsAct_->setStatusTip(tr("Download Annotations to Desktop"));
  } connect(downloadAnnotationsAct_, SIGNAL(triggered()), SLOT(downloadAnnotations()));
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
  QMenu *m = new QMenu;

  currentUrl_ = hoveredLink();
  QString selectedLink = selectedUrl();
  if (currentUrl_.isEmpty()) {
    currentUrl_ = selectedLink;
    if (currentUrl_.isEmpty())
      currentUrl_ = url().toString();
  }
  int site = MrlAnalysis::matchSite(currentUrl_);
  if (site) {
    m->addAction(openWithAcPlayerAct_);
    if (site < MrlAnalysis::AnnotationSite)
      m->addAction(importToAcPlayerAct_);
    if (site < MrlAnalysis::ChineseVideoSite) // TODO: change to all sites after fixing youtube
      m->addAction(openWithAcDownloaderAct_);
    if (site < MrlAnalysis::AnnotationSite)
      m->addAction(downloadAnnotationsAct_);
    m->addSeparator();
  }
  if (!selectedLink.isEmpty()) {
    m->addAction(openSelectedLinkAct_);
    m->addSeparator();
  }

  QMenu *searchMenu = nullptr;
  QString selection = selectedText().simplified();
  if (!selection.isEmpty() && hasSearchEngines()) {
    searchMenu = new QMenu(tr("Search") + " ...");
    for (int engine = 0; engine < searchEngines_.size(); engine++) {
      QtExt::ActionWithId *a = new QtExt::ActionWithId(engine, searchMenu);
      SearchEngine *e = searchEngines_[engine];
      //a->setText(tr("Search with %1").arg(e->name()));
      a->setText(e->name());
      a->setStatusTip(e->search("@key"));
      a->setIcon(QIcon(e->icon()));
      a->setCheckable(true);
      if (engine == searchEngine_)
        a->setChecked(true);
      connect(a, SIGNAL(triggeredWithId(int,bool)), SLOT(searchWithEngine(int)));

      switch (engine) {
      case SearchEngineFactory::Youtube:
      case SearchEngineFactory::WikiJa:
        searchMenu->addSeparator();
      }
      searchMenu->addAction(a);
    }

    int engine = SearchEngineFactory::Qt;
    if (engine >= searchEngines_.size() && selection.startsWith('Q') && !selection.contains(' ')) {
      QtExt::ActionWithId *a = new QtExt::ActionWithId(engine, m);
      static SearchEngine *e = nullptr;
      if (!e)
        e = SearchEngineFactory::globalInstance()->create(engine);
      //a->setText(tr("Search with %1").arg(e->name()));
      a->setText(e->name());
      a->setStatusTip(e->search("@key"));
      a->setIcon(QIcon(e->icon()));
      a->setCheckable(true);
      if (engine == searchEngine_)
        a->setChecked(true);
      connect(a, SIGNAL(triggeredWithId(int,bool)), SLOT(searchWithEngine(int)));
      searchMenu->addAction(a);
    }

    m->addMenu(searchMenu);
    m->addSeparator();
  }

  QMenu *scm = page()->createStandardContextMenu();
  m->addActions(scm->actions());
  //QAction *a;
  //if (a = page()->action(QWebPage::ReloadAndBypassCache))
  //  m->addAction(a);

  m->addSeparator();
  QMenu *history = createHistoryMenu();
  if (history) {
    m->addMenu(history);
    m->addSeparator();
  }
  m->addSeparator();
  m->addAction(clipAct);
  m->addAction(undoClosedTabAct_);
  m->addAction(clearHighlightAct);
  //m->addSeparator();
  //m->addAction(zoomResetAct);
  //m->addAction(zoomInAct);
  //m->addAction(zoomOutAct);
  m->addSeparator();
  m->addAction(openWithOperatingSystemAct);
  m->addAction(newWindowAct_);
  m->addAction(fullScreenAct_);
#ifdef Q_OS_WIN
  m->addAction(menuBarAct_);
#endif // Q_OS_WIN

  m->exec(event->globalPos());
  delete m;
  if (searchMenu)
    delete searchMenu;
  //delete scm; // FIXME
  if (history)
    delete history;
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
