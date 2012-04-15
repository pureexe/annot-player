// wbwebview.cc
// 1/27/2012
#include "wbwebview.h"
#include "ac/acrc.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include <QMenu>
#include <QContextMenuEvent>

// - Construction -

void
WbWebView::createActions()
{
  openWithAcPlayerAct_ = new QAction(this); {
    openWithAcPlayerAct_->setIcon(QIcon(ACRC_IMAGE_PLAYER));
    openWithAcPlayerAct_->setText(tr("Play with Annot Player"));
    connect(openWithAcPlayerAct_, SIGNAL(triggered()), SLOT(openWithAcPlayer()));
  }
  openWithAcDownloaderAct_ = new QAction(this); {
    openWithAcDownloaderAct_->setIcon(QIcon(ACRC_IMAGE_DOWNLOADER));
    openWithAcDownloaderAct_->setText(tr("Download with Annot Downloader"));
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
  QMenu m;
  if (MrlAnalysis::matchSite(currentUrl_ = hoveredLink())) {
    m.addAction(openWithAcPlayerAct_);
    m.addAction(openWithAcDownloaderAct_);
    m.addSeparator();
  }

  QMenu *scm = page()->createStandardContextMenu();
  m.addActions(scm->actions());

  m.addSeparator();
  m.addAction(undoClosedTabAct_);
  m.addSeparator();
  m.addAction(zoomInAct);
  m.addAction(zoomOutAct);
  m.addAction(zoomResetAct);

  m.exec(event->globalPos());
  delete scm;
}

// EOF
