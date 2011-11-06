// pickdialog.cc
// 10/30/2011

#include "pickdialog.h"
#include "uistyle.h"
#include "tr.h"
#include "stylesheet.h"
#include "logger.h"
#include "global.h"
#ifdef USE_WIN_PICKER
  #include "win/picker/picker.h"
#endif USE_WIN_PICKER
#include "core/gui/toolbutton.h"
#include <QtGui>

#define DEBUG "PickDialog"
#include "module/debug/debug.h"

using namespace Logger;

// - Panel -
PickDialog::PickDialog(QWidget *parent)
  : Base(parent), active_(false)
{
  setWindowTitle(tr("Select window"));
  UiStyle::globalInstance()->setWindowStyle(this);
  setContentsMargins(0, 0, 0, 0);

  // Widgets

  messageLabel_ = new QLabel; {
    messageLabel_->setStyleSheet(SS_LABEL);
    messageLabel_->setToolTip(tr("Message"));
  }

  QToolButton *cancelButton = new Core::Gui::ToolButton; {
    cancelButton->setStyleSheet(SS_TOOLBUTTON_TEXT);
    cancelButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    cancelButton->setText(QString("[ %1 ]").arg(TR(T_CANCEL)));
    cancelButton->setToolTip(TR(T_CANCEL));
  }

  // Layouts
  QVBoxLayout *layout = new QVBoxLayout; {
    layout->addWidget(messageLabel_, 0, Qt::AlignCenter);
    layout->addWidget(cancelButton, 0, Qt::AlignCenter);
  }
  setLayout(layout);

  // Connections
  connect(cancelButton, SIGNAL(clicked()), SLOT(cancel()));

  // Focus
  cancelButton->setFocus();
}

bool
PickDialog::isActive() const
{ return active_; }


void
PickDialog::setActive(bool t)
{
  if (active_ != t) {
#ifdef USE_WIN_PICKER
    if (t)
      connect(PICKER, SIGNAL(windowPicked(WId)), SLOT(pickWindow(WId)));
    else
      disconnect(PICKER, SIGNAL(windowPicked(WId)), this, SLOT(pickWindow(WId)));
#endif USE_WIN_PICKER
    active_ = t;
  }
}

// - Properties -

void
PickDialog::setMessage(const QString &text)
{ messageLabel_->setText(text); }

// - Slots -

void
PickDialog::cancel()
{
  DOUT("cancel:enter");
  hide();
  log(tr("window picking canceled"));
  DOUT("cancel:exit");
}

void
PickDialog::pickWindow(WId winId)
{
  DOUT("pickWindow:enter");
  if (winId == this->winId()) {
    startPicking();
    DOUT("pickWindow:exit: skip picking myself");
    return;
  }

  log(tr("window picked") + QString(" (hwnd = %1)").arg(QString::number((ulong)winId, 16)));

  hide();
  emit windowPicked(winId);
  DOUT("pickWindow:exit");
}

void
PickDialog::startPicking()
{
  DOUT("startPicking:enter");
#ifdef USE_WIN_PICKER
  PICKER->singleShot();
#endif USE_WIN_PICKER
  DOUT("startPicking:exit");
}

void
PickDialog::stopPicking()
{
  DOUT("startPicking:enter");
#ifdef USE_WIN_PICKER
  PICKER->stop();
#endif USE_WIN_PICKER
  DOUT("startPicking:exit");
}

// - Events -

void
PickDialog::setVisible(bool visible)
{
  if (visible == isVisible())
    return;

  if (visible)
    startPicking();
  else
    stopPicking();

  setActive(visible);
  Base::setVisible(visible);
}

void
PickDialog::keyPressEvent(QKeyEvent *event)
{
  if (event)
    switch (event->key()) {
    case Qt::Key_Escape:
    case Qt::Key_Enter:
    case Qt::Key_Space:
      cancel();
      return;
    }

   Base::keyPressEvent(event);
}

// EOF
