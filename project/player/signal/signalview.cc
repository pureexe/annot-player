// signalview.cc
// 10/16/2011

#include "signalview.h"
//#include "tokenview.h"
#include "messageview.h"
#include "processview.h"
#include "tr.h"
#include "ac/acui.h"
#include <QtGui>

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

SignalView::SignalView(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(TR(T_TITLE_SIGNALVIEW));
  setContentsMargins(0, 0, 0, 0);
  setAcceptDrops(true);

  createLayout();
}

void
SignalView::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);

  // Views
  processView_ = new ProcessView;
  messageView_ = new MessageView;
  //tokenView_ = new TokenView;

  processView_->setWindowFlags(Qt::Widget);
  messageView_->setWindowFlags(Qt::Widget);
  //tokenView_->setWindowFlags(Qt::Widget);
  processView_->setDraggable(false);
  messageView_->setDraggable(false);
  //tokenView_->setDraggable(false);

  // Buttons

  QToolButton *processButton = ui->makeToolButton(
      AcUi::TabHint, tr("process"), processView_->windowTitle(), processView_, SLOT(setVisible(bool)));
  QToolButton *messageButton = ui->makeToolButton(
      AcUi::TabHint, tr("message"), messageView_->windowTitle(), messageView_, SLOT(setVisible(bool)));
  processButton->setChecked(true);
  messageButton->setChecked(true);

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *center = new QHBoxLayout;
    rows->addLayout(header);
    rows->addLayout(center);

    //header->addWidget(tokenButton);
    header->addWidget(processButton);
    header->addWidget(messageButton);

    //center->addWidget(tokenView_);
    center->addWidget(processView_);
    center->addWidget(messageView_);

    // left, top, right, bottom
    //tokenView_->layout()->setContentsMargins(4, 9, 4, 9);
    processView_->layout()->setContentsMargins(4, 9, 4, 9);
    messageView_->layout()->setContentsMargins(4, 6, 4, 9);
    header->setContentsMargins(0, 0, 0, 0);
    center->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(4, 0, 4, 4);
  } setLayout(rows);

  connect(messageView_, SIGNAL(hookSelected(ulong)), SLOT(selectHookAndHide(ulong)));

  connect(processView_, SIGNAL(attached(ProcessInfo)), messageView_, SLOT(setProcessNameFromProcessInfo(ProcessInfo)));
  connect(processView_, SIGNAL(detached(ProcessInfo)), messageView_, SLOT(clearProcessName()));

  connect(messageView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(messageView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(messageView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(messageView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

  connect(processView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(processView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(processView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(processView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

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
//SignalView::tokenView() const
//{ return tokenView_; }

// - Events -

void
SignalView::selectHookAndHide(ulong hookId)
{
  hide();
  ProcessInfo pi = processView_->attachedProcessInfo();
  emit hookSelected(hookId, pi);
}

void
SignalView::setVisible(bool visible)
{
  if (visible != messageView_->isActive())
    messageView_->setActive(visible);
  Base::setVisible(visible);
}

void SignalView::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
void SignalView::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
void SignalView::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
void SignalView::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

// EOF
