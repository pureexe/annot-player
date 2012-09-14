// timeinputdialog.cc
// 8/16/2011

#include "timeinputdialog.h"
#include "tr.h"
#include "global.h"
#include "src/common/acui.h"
#include "src/common/actimeedit.h"
#include "lib/qtext/datetime.h"
#include <QtGui>

#define DEBUG "timeinputdialog"
#include "lib/debug/debug.h"

enum { EDIT_MAXWIDTH = 50 };
enum { BAD_TIME = -1 };

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowStaysOnTopHint

#ifdef Q_OS_MAC
# define WINDOW_FLAGS  WINDOW_FLAGS_BASE | Qt::FramelessWindowHint
#else
# define WINDOW_FLAGS  WINDOW_FLAGS_BASE | Qt::WindowCloseButtonHint
#endif // Q_OS_MAC

// - Constructions -

TimeInputDialog::TimeInputDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  createLayout();
  timeEdit_->setFocus();
}

void
TimeInputDialog::createLayout()
{
  AcTimeEdit *timeEdit = new AcTimeEdit;
  timeEdit->setToolTip(TR(T_TIME));
  connect(timeEdit, SIGNAL(returnPressed()), SLOT(ok()));
  timeEdit_ = timeEdit;

  QToolButton *okButton = AcUi::globalInstance()->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_OK), this, SLOT(ok()));
  QToolButton *cancelButton = AcUi::globalInstance()->makeToolButton(
        AcUi::PushHint, TR(T_CANCEL), this, SLOT(fadeOut()));

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *buttons = new QHBoxLayout;
    rows->addWidget(timeEdit_);
    rows->addLayout(buttons);

    buttons->addWidget(cancelButton);
    buttons->addWidget(okButton);

    buttons->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(9, 9, 9, 9);
    //setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);
}

// - Properties -

qint64
TimeInputDialog::time() const
{
  return timeEdit_->time().msecsTo(QTime());
  //return ss * 1000 + mm * 1000 * 60 + hh * 1000 * 60 * 60;
}

void
TimeInputDialog::setTime(qint64 msecs)
{
  QTime t = QtExt::msecs2time(msecs);
  if (!t.isValid())
    return;
  timeEdit_->setTime(t);
}

// - Slots -

void
TimeInputDialog::ok()
{
  qint64 msecs = time();
  if (msecs < 0) {
    emit warning(TR(T_ERROR_BAD_POS));
    return;
  }
  //fadeOut();
  emit timeChanged(msecs);
}

// EOF
