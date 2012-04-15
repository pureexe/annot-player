// pickdialog.cc
// 10/30/2011

#include "pickdialog.h"
#include "tr.h"
#include "logger.h"
#ifdef WITH_WIN_PICKER
#  include "win/picker/picker.h"
#endif // WITH_WIN_PICKER
#include "ac/acui.h"
#include <QtGui>

#define DEBUG "pickdialog"
#include "module/debug/debug.h"

using namespace Logger;

#define PICKER  WindowPicker::globalInstance()

// - Constructions -

PickDialog::PickDialog(QWidget *parent)
  : Base(parent), active_(false)
{
#ifdef Q_WS_MAC
  setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
#endif // Q_WS_MAC
  setWindowTitle(tr("Select window"));

  createLayout();
}

void
PickDialog::createLayout()
{
  AcUi::globalInstance()->setWindowStyle(this);

  messageLabel_ = AcUi::globalInstance()->makeLabel(0, "", TR(T_MESSAGE));

  QToolButton *cancelButton = AcUi::globalInstance()->makeToolButton(
        AcUi::PushHint, TR(T_CANCEL), this, SLOT(cancel()));

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    rows->addWidget(messageLabel_, 0, Qt::AlignCenter);
    rows->addWidget(cancelButton, 0, Qt::AlignCenter);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);

  // Connections
  connect(cancelButton, SIGNAL(clicked()), SLOT(cancel()));

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(cancel()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence("CTRL+W"), this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(cancel()));

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
#ifdef WITH_WIN_PICKER
    if (t)
      connect(PICKER, SIGNAL(windowPicked(WId)), SLOT(pickWindow(WId)));
    else
      disconnect(PICKER, SIGNAL(windowPicked(WId)), this, SLOT(pickWindow(WId)));
#endif // WITH_WIN_PICKER
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
  DOUT("enter");
  hide();
  log(tr("window picking canceled"));
  DOUT("exit");
}

void
PickDialog::pickWindow(WId winId)
{
  DOUT("enter");
  if (winId == this->winId()) {
    startPicking();
    DOUT("exit: skip picking myself");
    return;
  }

  log(tr("window picked") + QString(" (hwnd = %1)").arg(QString::number((ulong)winId, 16)));

  hide();
  emit windowPicked(winId);
  DOUT("exit");
}

void
PickDialog::startPicking()
{
  DOUT("enter");
#ifdef WITH_WIN_PICKER
  PICKER->singleShot();
#endif // WITH_WIN_PICKER
  DOUT("exit");
}

void
PickDialog::stopPicking()
{
  DOUT("enter");
#ifdef WITH_WIN_PICKER
  PICKER->stop();
#endif // WITH_WIN_PICKER
  DOUT("exit");
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
