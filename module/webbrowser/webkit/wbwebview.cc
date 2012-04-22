// wbwebview.cc
// 1/27/2012
#include "wbwebview.h"
#include "wbrc.h"
#include "ac/acrc.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QTimer>

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around assignment
#endif // __GNUC__

// - Construction -

WbWebView::WbWebView(QWidget *parent)
  : Base(parent)
{ createActions(); }

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
    undoClosedTabAct_->setText(tr("Undo closed tab"));
    connect(undoClosedTabAct_, SIGNAL(triggered()), SIGNAL(undoClosedTabRequested()));
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
  Q_ASSERT(event);
  QMenu *m = new QMenu(this);

  currentUrl_ = hoveredLink();
  if (currentUrl_.isEmpty())
    currentUrl_ = url().toString();
  MrlAnalysis::Site site = MrlAnalysis::matchSite(currentUrl_);
  if (site) {
    m->addAction(openWithAcPlayerAct_);
    if (site < MrlAnalysis::AnnotationSite)
      m->addAction(importToAcPlayerAct_);
    if (site < MrlAnalysis::ChineseVideoSite) // TODO: change to all sites after fixing youtube
      m->addAction(openWithAcDownloaderAct_);
    m->addSeparator();
  }

  QMenu *scm = page()->createStandardContextMenu();
  m->addActions(scm->actions());
  //QAction *a;
  //if (a = page()->action(QWebPage::ReloadAndBypassCache))
  //  m->addAction(a);

  m->addSeparator();
  m->addAction(clipAct);
  m->addAction(undoClosedTabAct_);
  m->addSeparator();
  m->addAction(zoomInAct);
  m->addAction(zoomOutAct);
  m->addAction(zoomResetAct);

  m->exec(event->globalPos());
  delete scm;
  QTimer::singleShot(0, m, SLOT(deleteLater()));
  event->accept();
}

// EOF
