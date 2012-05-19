// gui/wbaddressedit.cc
// 3/31/2012

#include "gui/wbaddressedit.h"
#include "global/wbrc.h"
#include "global/wbss.h"
#include "project/common/acrc.h"
#include "module/mrlanalysis/mrlanalysis.h"
#include <QtGui>

// - Construction -

void
WbAddressEdit::init()
{
  setIcon(WBRC_IMAGE_APP);

  createActions();
  createConnections();
  lineEdit()->setPlaceholderText(tr("Go to this address"));
}

void
WbAddressEdit::createActions()
{
  submitAct->setText(tr("Go to This Address"));
  submitAct->setStatusTip(tr("Go to This Address"));

  pasteAndGoAct->setText(tr("Paste and Go"));
  pasteAndGoAct->setStatusTip(tr("Paste and Go"));

  openAddressWithAcPlayerAct_ = new QAction(this); {
    openAddressWithAcPlayerAct_->setIcon(QIcon(ACRC_IMAGE_PLAYER));
    openAddressWithAcPlayerAct_->setText(tr("Play with Annot Player"));
    openAddressWithAcPlayerAct_->setStatusTip(tr("Open with Annot Player"));
  } connect(openAddressWithAcPlayerAct_, SIGNAL(triggered()), SLOT(openWithAcPlayer()));
  importAddressToAcPlayerAct_ = new QAction(this); {
    importAddressToAcPlayerAct_->setIcon(QIcon(WBRC_IMAGE_COMMENT));
    importAddressToAcPlayerAct_->setText(tr("Import Annotations to Annot Player"));
    importAddressToAcPlayerAct_->setStatusTip(tr("Import Annotations to Annot Player"));
  } connect(importAddressToAcPlayerAct_, SIGNAL(triggered()), SLOT(importToAcPlayer()));
  openAddressWithAcDownloaderAct_ = new QAction(this); {
    openAddressWithAcDownloaderAct_->setIcon(QIcon(ACRC_IMAGE_DOWNLOADER));
    openAddressWithAcDownloaderAct_->setText(tr("Download with Annot Downloader"));
    openAddressWithAcDownloaderAct_->setStatusTip(tr("Open with Annot Downloader"));
  } connect(openAddressWithAcDownloaderAct_, SIGNAL(triggered()), SLOT(openWithAcDownloader()));

  openWithOperatingSystemAct_ = new QAction(this); {
    openWithOperatingSystemAct_->setText(tr("Open in System Default Browser"));
    openWithOperatingSystemAct_->setStatusTip(tr("Open in System Default Browser"));
  } connect(openWithOperatingSystemAct_, SIGNAL(triggered()), SLOT(openWithOperatingSystem()));
}

void
WbAddressEdit::createConnections()
{
  connect(this, SIGNAL(activated(int)), SLOT(submitText()));
  connect(this, SIGNAL(styleSheetChanged()), SLOT(updatePalette()));
}

// - Events -

void
WbAddressEdit::contextMenuEvent(QContextMenuEvent *event)
{
  if (!event)
    return;

  QMenu *m = new QMenu(this);

  int site = MrlAnalysis::matchSite(currentText().trimmed(), false); // href = false

  if (site) {
    m->addAction(openAddressWithAcPlayerAct_);
    if (site < MrlAnalysis::AnnotationSite)
      m->addAction(importAddressToAcPlayerAct_);
    if (site < MrlAnalysis::ChineseVideoSite) // TODO: change to all sites after fixing youtube
      m->addAction(openAddressWithAcDownloaderAct_);
    m->addSeparator();
  }

  //m->addAction(submitAct);
  m->addAction(pasteAndGoAct);
  m->addAction(popupAct);
  m->addAction(clearAct);
  m->addAction(openWithOperatingSystemAct_);
  m->addSeparator();

  pasteAndGoAct->setEnabled(!isClipboardEmpty());
  popupAct->setEnabled(count());
  openWithOperatingSystemAct_->setEnabled(!currentText().simplified().isEmpty());

  QMenu *scm = lineEdit()->createStandardContextMenu();
  m->addActions(scm->actions());

  m->exec(event->globalPos());
  delete scm;
  delete m;
  event->accept();
}

// - Properties -

void
WbAddressEdit::setProgress(int value)
{
  if (progress_ != value) {
    progress_ = value;
    updatePalette();
  }
}

void
WbAddressEdit::updatePalette()
{
  if (progress_ == 100)
    setStyleSheet(styleSheet());
  else if (int w = width()) {
#define X_INIT  0.1
    qreal x = X_INIT + progress_ *((1 - X_INIT)/100.0);
#undef X_INIT
    QLinearGradient g(0, 0, w*x, 0); // horizental
    g.setColorAt(0, Qt::transparent);
    g.setColorAt(0.99, QColor(113, 201, 244, 180)); // #71c9f4
    g.setColorAt(1, Qt::transparent);
    QPalette p;
    p.setBrush(QPalette::Base, g);
    lineEdit()->setPalette(p);
  }
}

// EOF

/*
void
WbAddressEdit::keyPressEvent(QKeyEvent *event)
{
  Q_ASSERT(event);
  // Do not pass escape key to parent.
  switch (event->key()) {
  case Qt::Key_Escape:
    DOUT("Key_Escape");
    clearFocus();    // FIXME: after clear focus, which window get new focus?
    event->accept();
    return;

  case Qt::Key_Return:
    DOUT("Key_Return");
    event->accept();
    //emit returnPressed();
    return;

  case Qt::Key_Up:
    DOUT("Key_Up");
    //previous();
    event->accept();
    return;

  case Qt::Key_Down:
    DOUT("Key_Down");
    //next();
    event->accept();
    return;
  }
  Base::keyPressEvent(event);
}
*/
