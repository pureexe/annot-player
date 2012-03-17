// countdowndialog.cc
// 2/13/2012

#include "countdowndialog.h"
#include "uistyle.h"
#include "tr.h"
#include "stylesheet.h"
#include "logger.h"
#include "module/qtext/toolbutton.h"
#include <QtGui>

using namespace Logger;

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowStaysOnTopHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint

#ifdef Q_WS_MAC
  #define WINDOW_FLAGS ( \
    Qt::FramelessWindowHint | \
    WINDOW_FLAGS_BASE )
#else
  #define WINDOW_FLAGS ( \
    WINDOW_FLAGS_BASE )
#endif // Q_WS_MAC

// - Constructions -

CountdownDialog::CountdownDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), count_(DefaultCount), currentCount_(0)
{
  setWindowTitle(tr("Countdown"));
  UiStyle::globalInstance()->setWindowStyle(this);

  timer_ = new QTimer(this);
  timer_->setInterval(1000); // 1 second
  connect(timer_, SIGNAL(timeout()), SLOT(tick()));

  createLayout();
}

void
CountdownDialog::createLayout()
{
#define MAKE_BUTTON(_button, _title, _tip, _slot) \
  QToolButton *_button = new QtExt::ToolButton; { \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _button->setText(QString("[ %1 ]").arg(_title)); \
    _button->setToolTip(_tip); \
  } connect(_button, SIGNAL(clicked()), _slot);

  MAKE_BUTTON(okButton, TR(T_OK), TR(T_OK), SLOT(run()))
  MAKE_BUTTON(cancelButton, TR(T_CANCEL), TR(T_CANCEL), SLOT(cancel()))
  okButton->setStyleSheet(SS_TOOLBUTTON_TEXT_HIGHLIGHT);

  messageLabel_ = new QLabel; {
    messageLabel_->setStyleSheet(SS_LABEL);
    messageLabel_->setToolTip(TR(T_MESSAGE));
  }

  QVBoxLayout *rows = new QVBoxLayout; {
    QLayout *buttons = new QHBoxLayout;

    rows->addWidget(messageLabel_);
    rows->addLayout(buttons);

    buttons->addWidget(cancelButton);
    buttons->addWidget(okButton);

    rows->setContentsMargins(9, 9, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);

#undef MAKE_BUTTON

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(cancel()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(cancel()));

  cancelButton->setFocus();
}

// - Actions -

void
CountdownDialog::start()
{
  warn(messageLabel_->text());
  currentCount_ = count_;
  timer_->start();
}

void
CountdownDialog::stop()
{
  log(tr("countdown stopped"));
  timer_->stop();
}

void
CountdownDialog::setVisible(bool visible)
{
  if (visible) {
    start();
    refresh();
  } else
    stop();
  Base::setVisible(visible);
}

void
CountdownDialog::refresh()
{ messageLabel_->setText(message_.arg(QString::number(currentCount_))); }

void
CountdownDialog::tick()
{
  if (--currentCount_ <= 0) {
    stop();
    run();
  }
  refresh();
}

// EOF
