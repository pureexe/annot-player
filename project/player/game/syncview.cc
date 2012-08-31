// syncview.cc
// 10/16/2011

#include "syncview.h"
//#include "tokenview.h"
#include "messageview.h"
#include "processview.h"
#include "tr.h"
#include "project/common/acui.h"
#include "project/common/acprotocol.h"
#include "module/qtext/layoutwidget.h"
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
  messageView_ = new MessageView(messageHandler_, this);
  //tokenView_ = new TokenView(this);

  processView_->setWindowFlags(Qt::Widget);
  messageView_->setWindowFlags(Qt::Widget);
  //tokenView_->setWindowFlags(Qt::Widget);
  processView_->setDraggable(false);
  messageView_->setDraggable(false);
  //tokenView_->setDraggable(false);

  // Buttons

  processButton_ = ui->makeToolButton(
      AcUi::TabHint, tr("process"), processView_->windowTitle(), processView_, SLOT(setVisible(bool)));
  messageButton_ = ui->makeToolButton(
      AcUi::TabHint, tr("thread"), messageView_->windowTitle(), messageView_, SLOT(setVisible(bool)));
  processButton_->setChecked(true);
  messageButton_->setChecked(true);

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *center = new QHBoxLayout;
    rows->addLayout(header);
    rows->addLayout(center);

    //header->addWidget(tokenButton);
    header->addWidget(processButton_);
    header->addWidget(messageButton_);

    //center->addWidget(tokenView_);
    center->addWidget(processView_);
    center->addWidget(messageView_);

    // left, top, right, bottom
    //tokenView_->layout()->setContentsMargins(4, 9, 4, 9);
    processView_->layout()->setContentsMargins(4, 9, 4, 9);
    messageView_->layout()->setContentsMargins(4, 6, 4, 9);
    header->setContentsMargins(0, 0, 0, 0);
    center->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
  } setCentralWidget(new LayoutWidget(rows, this));

  connect(messageView_, SIGNAL(threadsSelected(TextThreadList)), SLOT(select(TextThreadList)));

  connect(processView_, SIGNAL(attached(ProcessInfo)), messageView_, SLOT(setProcessNameFromProcessInfo(ProcessInfo)));
  connect(processView_, SIGNAL(detached(ProcessInfo)), messageView_, SLOT(clear()));

  connect(processView_, SIGNAL(attached(ProcessInfo)), SLOT(showMessageView()));
  connect(processView_, SIGNAL(detached(ProcessInfo)), SLOT(disableMessageView()));

  AC_FORWARD_MESSAGES(messageView_, this, Qt::AutoConnection);
  AC_FORWARD_MESSAGES(processView_, this, Qt::AutoConnection);

  //connect(messageView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  //connect(messageView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  //connect(messageView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  //connect(messageView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

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
//  //if (visible != messageView_->isActive())
//  //  messageView_->setActive(visible);
//  Base::setVisible(visible);
//}

void
SyncView::setProcessViewVisible(bool t)
{
  processButton_->setChecked(t);
  processView_->setVisible(t);
}

void
SyncView::setMessageViewVisible(bool t)
{
  messageButton_->setChecked(t);
  messageView_->setVisible(t);
}

void
SyncView::showMessageView()
{
  messageView_->setEnabled(true);
  setMessageViewVisible(true);
}

void
SyncView::disableMessageView()
{ messageView_->setEnabled(false); }

//void SyncView::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
//void SyncView::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
//void SyncView::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
//void SyncView::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

// EOF
