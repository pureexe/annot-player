// seekdialog.cc
// 8/16/2011

#include "seekdialog.h"
#include "tr.h"
#include "global.h"
#include "project/common/acui.h"
#include "project/common/actimeedit.h"
#include "module/qtext/datetime.h"
#include <QtGui>

#define DEBUG "seekdialog"
#include "module/debug/debug.h"

enum { EDIT_MAXWIDTH = 50 };
enum { BAD_TIME = -1 };

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowStaysOnTopHint

#ifdef Q_OS_MAC
#  define WINDOW_FLAGS  WINDOW_FLAGS_BASE | Qt::FramelessWindowHint
#else
#  define WINDOW_FLAGS  WINDOW_FLAGS_BASE | Qt::WindowCloseButtonHint
#endif // Q_OS_MAC

// - Constructions -

SeekDialog::SeekDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(TR(T_TITLE_SEEK));

  createLayout();
  timeEdit_->setFocus();
}

void
SeekDialog::createLayout()
{
  AcTimeEdit *timeEdit = new AcTimeEdit;
  timeEdit->setToolTip(TR(T_TITLE_SEEK));
  connect(timeEdit, SIGNAL(returnPressed()), SLOT(ok()));
  timeEdit_ = timeEdit;

  QToolButton *okButton = AcUi::globalInstance()->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_SEEK), this, SLOT(ok()));
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
SeekDialog::time() const
{
  return timeEdit_->time().msecsTo(QTime());
  //return ss * 1000 + mm * 1000 * 60 + hh * 1000 * 60 * 60;
}

void
SeekDialog::setTime(qint64 msecs)
{
  QTime t = QtExt::msecs2time(msecs);
  if (!t.isValid())
    return;
  timeEdit_->setTime(t);
}

// - Slots -

void
SeekDialog::ok()
{
  qint64 msecs = time();
  if (msecs < 0) {
    emit warning(TR(T_ERROR_BAD_POS));
    return;
  }
  //fadeOut();
  emit seekRequested(msecs);
}

// EOF
