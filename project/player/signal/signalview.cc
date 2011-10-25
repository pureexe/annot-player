// signalview.cc
// 10/16/2011

#include "signalview.h"
#include "tokenview.h"
#include "messageview.h"
#include "processview.h"
#include "uistyle.h"
#include "stylesheet.h"
#include "core/gui/toolbutton.h"
#include "tr.h"
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
  UiStyle::globalInstance()->setWindowStyle(this);
  setContentsMargins(0, 0, 0, 0);

  // Views
  processView_ = new ProcessView;
  messageView_ = new MessageView;
  tokenView_ = new TokenView;

  processView_->setWindowFlags(Qt::Widget);
  messageView_->setWindowFlags(Qt::Widget);
  tokenView_->setWindowFlags(Qt::Widget);
  processView_->setDraggable(false);
  messageView_->setDraggable(false);
  tokenView_->setDraggable(false);

  // Buttons

#define MAKE_BUTTON(_id, _text) \
  QToolButton *_id##Button = new Core::Gui::ToolButton; { \
    _id##Button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _id##Button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _id##Button->setCheckable(true); \
    _id##Button->setChecked(true); \
    _id##Button->setText(QString("- %1 -").arg(_text)); \
    _id##Button->setToolTip(_id##View_->windowTitle()); \
  } \
  connect(_id##Button, SIGNAL(clicked(bool)), _id##View_, SLOT(setVisible(bool)));

  MAKE_BUTTON(token, tr("info"))
  MAKE_BUTTON(process, tr("process"))
  MAKE_BUTTON(message, tr("message"))
#undef MAKE_BUTTON

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *center = new QHBoxLayout;
    rows->addLayout(header);
    rows->addLayout(center);

    header->addWidget(tokenButton);
    header->addWidget(processButton);
    header->addWidget(messageButton);

    center->addWidget(tokenView_);
    center->addWidget(processView_);
    center->addWidget(messageView_);

    // left, top, right, bottom
    tokenView_->layout()->setContentsMargins(4, 9, 4, 9);
    processView_->layout()->setContentsMargins(4, 9, 4, 9);
    messageView_->layout()->setContentsMargins(4, 6, 4, 9);
    header->setContentsMargins(0, 0, 0, 0);
    center->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(4, 0, 0, 4);
  }
  setLayout(rows);

  connect(messageView_, SIGNAL(hookSelected(int)), SLOT(selectHookAndHide(int)));

  connect(messageView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(messageView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(messageView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(messageView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

  connect(processView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(processView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(processView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(processView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

  connect(tokenView_, SIGNAL(dragEnterEventReceived(QDragEnterEvent*)), SLOT(dragEnterEvent(QDragEnterEvent*)));
  connect(tokenView_, SIGNAL(dragLeaveEventReceived(QDragLeaveEvent*)), SLOT(dragLeaveEvent(QDragLeaveEvent*)));
  connect(tokenView_, SIGNAL(dragMoveEventReceived(QDragMoveEvent*)), SLOT(dragMoveEvent(QDragMoveEvent*)));
  connect(tokenView_, SIGNAL(dropEventReceived(QDropEvent*)), SLOT(dropEvent(QDropEvent*)));

  // Initial states - hide TokenView by default -- TODO: remove this after tokenView works out of box
  //tokenButton->setCheckable(false);
  //tokenView_->hide();

  //processView_->setFocus();
}

TokenView*
SignalView::tokenView() const
{ return tokenView_; }

ProcessView*
SignalView::processView() const
{ return processView_; }

MessageView*
SignalView::messageView() const
{ return messageView_; }


// - Events -

void
SignalView::selectHookAndHide(int hookId)
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
