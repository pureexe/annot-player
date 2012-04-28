// wbwebview.cc
// 1/27/2012
#include "webkit/wbwebview.h"
#include "global/wbrc.h"
#include "core/wbsearchenginefactory.h"
#include "project/common/acrc.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include "module/qtext/actionwithid.h"
#include <QtGui>

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around assignment
#endif // __GNUC__

// - Construction -

WbWebView::WbWebView(QWidget *parent)
  : Base(parent), searchEngine_(0)
{
  createActions();
  connect(this, SIGNAL(selectionChanged()), SLOT(invalidateSelection()));

  connect(page(), SIGNAL(linkHovered(QString,QString,QString)), SLOT(highlightDownloadableLink(QString,QString,QString)));
}

void
WbWebView::createActions()
{
  openWithAcPlayerAct_ = new QAction(this); {
    openWithAcPlayerAct_->setIcon(QIcon(ACRC_IMAGE_PLAYER));
    openWithAcPlayerAct_->setText(tr("Play with Annot Player"));
    openWithAcPlayerAct_->setStatusTip(tr("Play with Annot Player"));
    connect(openWithAcPlayerAct_, SIGNAL(triggered()), SLOT(openWithAcPlayer()));
  }
  importToAcPlayerAct_ = new QAction(this); {
    importToAcPlayerAct_->setIcon(QIcon(WBRC_IMAGE_COMMENT));
    importToAcPlayerAct_->setText(tr("Import annotations to Annot Player"));
    importToAcPlayerAct_->setStatusTip(tr("Import annotations to Annot Player"));
    connect(importToAcPlayerAct_, SIGNAL(triggered()), SLOT(importToAcPlayer()));
  }
  openWithAcDownloaderAct_ = new QAction(this); {
    openWithAcDownloaderAct_->setIcon(QIcon(ACRC_IMAGE_DOWNLOADER));
    openWithAcDownloaderAct_->setText(tr("Download with Annot Downloader"));
    openWithAcDownloaderAct_->setStatusTip(tr("Download with Annot Downloader"));
    connect(openWithAcDownloaderAct_, SIGNAL(triggered()), SLOT(openWithAcDownloader()));
  }
  undoClosedTabAct_ = new QAction(this); {
    undoClosedTabAct_->setText(tr("Undo Close Tab"));
    undoClosedTabAct_->setStatusTip(tr("Undo Close Tab"));
    connect(undoClosedTabAct_, SIGNAL(triggered()), SIGNAL(undoClosedTabRequested()));
  }
  newWindowAct_ = new QAction(this); {
    newWindowAct_->setText(tr("New Window"));
    newWindowAct_->setStatusTip(tr("New Window"));
    newWindowAct_->setShortcut(QKeySequence::New);
    connect(newWindowAct_, SIGNAL(triggered()), SIGNAL(newWindowRequested()));
  }
  fullScreenAct_ = new QAction(this); {
    fullScreenAct_->setText(tr("Toggle Full Screen"));
    fullScreenAct_->setStatusTip(tr("Toggle Full Screen"));
    fullScreenAct_->setShortcut(QKeySequence("CTRL+META+F"));
    connect(fullScreenAct_, SIGNAL(triggered()), SIGNAL(fullScreenRequested()));
  }

  undoClosedTabAct_->setShortcut(QKeySequence("CTRL+SHIFT+T"));
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
  QMenu *m = new QMenu(this);

  currentUrl_ = hoveredLink();
  if (currentUrl_.isEmpty()) {
    currentUrl_ = selectedUrl();
    if (currentUrl_.isEmpty())
      currentUrl_ = url().toString();
  }
  MrlAnalysis::Site site = MrlAnalysis::matchSite(currentUrl_);
  if (site) {
    m->addAction(openWithAcPlayerAct_);
    if (site < MrlAnalysis::AnnotationSite)
      m->addAction(importToAcPlayerAct_);
    if (site < MrlAnalysis::ChineseVideoSite) // TODO: change to all sites after fixing youtube
      m->addAction(openWithAcDownloaderAct_);
    m->addSeparator();
  }

  QString selection = selectedText().simplified();
  if (!selection.isEmpty() && hasSearchEngines()) {

    for (int engine = 0; engine < searchEngines_.size(); engine++) {
      QtExt::ActionWithId *a = new QtExt::ActionWithId(engine, m);
      WbSearchEngine *e = searchEngines_[engine];
      a->setText(tr("Search with %1").arg(e->name()));
      a->setStatusTip(e->search("@key"));
      a->setIcon(QIcon(e->icon()));
      a->setCheckable(true);
      if (engine == searchEngine_)
        a->setChecked(true);
      connect(a, SIGNAL(triggeredWithId(int,bool)), SLOT(searchWithEngine(int)));
      m->addAction(a);
    }

    int engine = WbSearchEngineFactory::Qt;
    if (engine >= searchEngines_.size() && selection.startsWith('Q') && !selection.contains(' ')) {
      QtExt::ActionWithId *a = new QtExt::ActionWithId(engine, m);
      static WbSearchEngine *e = 0;
      if (!e)
        e = WbSearchEngineFactory::globalInstance()->create(engine);
      a->setText(tr("Search with %1").arg(e->name()));
      a->setStatusTip(e->search("@key"));
      a->setIcon(QIcon(e->icon()));
      a->setCheckable(true);
      if (engine == searchEngine_)
        a->setChecked(true);
      connect(a, SIGNAL(triggeredWithId(int,bool)), SLOT(searchWithEngine(int)));
      m->addAction(a);
    }

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
  m->addSeparator();
  m->addAction(zoomResetAct);
  m->addAction(zoomInAct);
  m->addAction(zoomOutAct);
  m->addSeparator();
  m->addAction(openWithOperatingSystemAct);
  m->addAction(newWindowAct_);
  m->addAction(fullScreenAct_);

  m->exec(event->globalPos());
  delete scm;
  QTimer::singleShot(0, m, SLOT(deleteLater()));
  if (history)
    QTimer::singleShot(0, history, SLOT(deleteLater()));
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
  if (ret.startsWith("http://")  || ret.startsWith("ttp://"))
    return ret;
  else
    return QString();
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
    QString m = QUrl::fromPercentEncoding(url.toLocal8Bit());
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
