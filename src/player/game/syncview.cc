// syncview.cc
// 10/16/2011

#include "syncview.h"
//#include "tokenview.h"
#include "threadview.h"
#include "processview.h"
#include "tr.h"
#include "src/common/acui.h"
#include "src/common/acprotocol.h"
#include "qtx/qxlayoutwidget.h"
#include <QtGui>

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

SyncView::SyncView(MessageHandler *h, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), messageHandler_(h)
{
  Q_ASSERT(messageHandler_);
  setWindowTitle(tr("Sync with Galgame"));
  setContentsMargins(0, 0, 0, 0);
  //setAcceptDrops(true);

  createLayout();
}

void
SyncView::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  // Views
  processView_ = new ProcessView(this);
  threadView_ = new ThreadView(messageHandler_, this);
  //tokenView_ = new TokenView(this);

  processView_->setWindowFlags(Qt::Widget);
  threadView_->setWindowFlags(Qt::Widget);
  //tokenView_->setWindowFlags(Qt::Widget);
  processView_->setDraggable(false);
  threadView_->setDraggable(false);
  //tokenView_->setDraggable(false);

  // Buttons

  processButton_ = ui->makeToolButton(
      AcUi::TabHint, tr("process"), processView_->windowTitle(), processView_, SLOT(setVisible(bool)));
  threadButton_ = ui->makeToolButton(
      AcUi::TabHint, tr("thread"), threadView_->windowTitle(), threadView_, SLOT(setVisible(bool)));
  processButton_->setChecked(true);
  threadButton_->setChecked(true);

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *center = new QHBoxLayout;
    rows->addLayout(header);
    rows->addLayout(center);

    //header->addWidget(tokenButton);
    header->addWidget(processButton_);
    header->addWidget(threadButton_);

    //center->addWidget(tokenView_);
    center->addWidget(processView_);
    center->addWidget(threadView_);

    // left, top, right, bottom
    //tokenView_->layout()->setContentsMargins(4, 9, 4, 9);
    processView_->layout()->setContentsMargins(4, 9, 4, 9);
    threadView_->layout()->setContentsMargins(4, 6, 4, 9);
    header->setContentsMargins(0, 0, 0, 0);
    center->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
  } setCentralWidget(new QxLayoutWidget(rows, this));

  connect(threadView_, SIGNAL(threadsSelected(TextThreadList)), SLOT(select(TextThreadList)));

  connect(processView_, SIGNAL(attached(ProcessInfo)), threadView_, SLOT(setProcessNameFromProcessInfo(ProcessInfo)));
  connect(processView_, SIGNAL(detached(ProcessInfo)), threadView_, SLOT(clear()));

  connect(processView_, SIGNAL(attached(ProcessInfo)), SLOT(showThreadView()));
  connect(processView_, SIGNAL(detached(ProcessInfo)), SLOT(disableThreadView()));

  AC_FORWARD_MESSAGES(threadView_, this, Qt::AutoConnection);
  AC_FORWARD_MESSAGES(processView_, this, Qt::AutoConnection);

  //connect(threadView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  //connect(threadView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  //connect(threadView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  //connect(threadView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

  //connect(processView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  //connect(processView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  //connect(processView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  //connect(processView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

  //connect(tokenView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  //connect(tokenView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  //connect(tokenView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  //connect(tokenView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

  // Initial states - hide TokenView by default -- TODO: remove this after tokenView works out of box
  //tokenButton->setCheckable(false);
  //tokenView_->hide();

  //processView_->setFocus();
}

//TokenView*
//SyncView::tokenView() const
//{ return tokenView_; }

// - Events -

void
SyncView::select(const TextThreadList &threads)
{
  if (threads.isEmpty())
    return;
  ProcessInfo pi = processView_->attachedProcessInfo();
  if (!pi.isValid()) {
    emit warning(tr("process is not attached properly. try retart the app first."));
    return;
  }
  emit threadsSelected(threads, pi);
  fadeOut();
}

//void
//SyncView::setVisible(bool visible)
//{
//  //if (visible != threadView_->isActive())
//  //  threadView_->setActive(visible);
//  Base::setVisible(visible);
//}

void
SyncView::setProcessViewVisible(bool t)
{
  processButton_->setChecked(t);
  processView_->setVisible(t);
}

void
SyncView::setThreadViewVisible(bool t)
{
  threadButton_->setChecked(t);
  threadView_->setVisible(t);
}

void
SyncView::showThreadView()
{
  threadView_->setEnabled(true);
  setThreadViewVisible(true);
}

void
SyncView::disableThreadView()
{ threadView_->setEnabled(false); }

//void SyncView::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
//void SyncView::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
//void SyncView::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
//void SyncView::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

// EOF
