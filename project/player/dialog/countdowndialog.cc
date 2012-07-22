// countdowndialog.cc
// 2/13/2012

#include "countdowndialog.h"
#include "tr.h"
#include "project/common/acui.h"
#include <QtGui>

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowStaysOnTopHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint

#ifdef Q_OS_MAC
  #define WINDOW_FLAGS ( \
    Qt::FramelessWindowHint | \
    WINDOW_FLAGS_BASE )
#else
  #define WINDOW_FLAGS ( \
    WINDOW_FLAGS_BASE )
#endif // Q_OS_MAC

// - Constructions -

CountdownDialog::CountdownDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), count_(DefaultCount), currentCount_(0)
{
  setWindowTitle(tr("Countdown"));

  timer_ = new QTimer(this);
  timer_->setInterval(1000); // 1 second
  connect(timer_, SIGNAL(timeout()), SLOT(tick()));

  createLayout();
}

void
CountdownDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  QToolButton *okButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_OK), this, SLOT(run()));
  QToolButton *cancelButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CANCEL), this, SLOT(cancel()));

  messageLabel_ = ui->makeLabel(0, "", TR(T_MESSAGE));

  QVBoxLayout *rows = new QVBoxLayout; {
    QLayout *buttons = new QHBoxLayout;

    rows->addWidget(messageLabel_);
    rows->addLayout(buttons);

    buttons->addWidget(cancelButton);
    buttons->addWidget(okButton);

    rows->setContentsMargins(9, 9, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);

  // Focus
  cancelButton->setFocus();
}

// - Actions -

void
CountdownDialog::start()
{
  emit notification(messageLabel_->text());
  currentCount_ = count_;
  timer_->start();
}

void
CountdownDialog::stop()
{
  emit message(tr("countdown stopped"));
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
