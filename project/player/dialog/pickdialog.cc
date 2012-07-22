// pickdialog.cc
// 10/30/2011

#include "pickdialog.h"
#include "tr.h"
#include "application.h"
#ifdef WITH_WIN_PICKER
# include "win/picker/picker.h"
#endif // WITH_WIN_PICKER
#include "project/common/acui.h"
#include <QtGui>

//#define DEBUG "pickdialog"
#include "module/debug/debug.h"

#define PICKER  WindowPicker::globalInstance()

// - Constructions -

#define WINDOW_SIZE QSize(150,50)

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint

#ifdef Q_OS_MAC
# define WINDOW_FLAGS  WINDOW_FLAGS_BASE | Qt::FramelessWindowHint
#else
# define WINDOW_FLAGS  WINDOW_FLAGS_BASE
#endif // Q_OS_MAC

PickDialog::PickDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), active_(false), pickedWindow_(0)
{
  setWindowTitle(tr("Select window"));
  resize(WINDOW_SIZE);
  createLayout();
}

void
PickDialog::createLayout()
{
  messageLabel_ = AcUi::globalInstance()->makeLabel(0, "", TR(T_MESSAGE));

  QToolButton *cancelButton = AcUi::globalInstance()->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint | AcUi::InvertHint,
        TR(T_CANCEL) + " (ESC)", this, SLOT(cancel()));

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    rows->addWidget(messageLabel_, 0, Qt::AlignCenter);
    rows->addWidget(cancelButton, 0, Qt::AlignCenter);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);

  // Connections
  connect(cancelButton, SIGNAL(clicked()), SLOT(cancel()));

  // Focus
  cancelButton->setFocus();
}

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
  //fadeOut();
  hide();
  emit message(tr("window picking canceled"));
  DOUT("exit");
}

void
PickDialog::pickWindow(WId hwnd)
{
  DOUT("enter");
  if (hwnd == winId()) {
    startPicking();
    DOUT("exit: skip picking myself");
    return;
  }

  emit message(tr("window picked") + QString(" (hwnd = %1)").arg(QString::number((ulong)hwnd, 16)));

  pickedWindow_ = hwnd;
  //fadeOut();
  hide();
  emit windowPicked(hwnd);
  DOUT("exit");
}

void
PickDialog::startPicking()
{
  DOUT("enter");
#ifdef WITH_WIN_PICKER
  PICKER->singleShot();
#endif // WITH_WIN_PICKER
  Application::globalInstance()->setCursor(Qt::PointingHandCursor);
  DOUT("exit");
}

void
PickDialog::stopPicking()
{
  DOUT("enter");
#ifdef WITH_WIN_PICKER
  PICKER->stop();
#endif // WITH_WIN_PICKER
  Application::globalInstance()->setCursor(Qt::ArrowCursor);
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

  if (visible)
    pickedWindow_ = 0; // reset picked window
  else if (!pickedWindow_)
    emit cancelled();
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
