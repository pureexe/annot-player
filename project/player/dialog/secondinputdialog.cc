// secondinputdialog.cc
// 6/12/2012

#include "secondinputdialog.h"
#include "tr.h"
#include "global.h"
#include "project/common/acui.h"
#include "module/qtext/datetime.h"
#include "module/qtext/spinbox.h"
#include <QtGui>

#define DEBUG "secondinputdialog"
#include "module/debug/debug.h"

enum { EDIT_WIDTH = 50 };
enum { BAD_TIME = -1 };

#define WINDOW_FLAGS_BASE \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint

#ifdef Q_OS_MAC
#  define WINDOW_FLAGS  WINDOW_FLAGS_BASE | Qt::FramelessWindowHint
#else
#  define WINDOW_FLAGS  WINDOW_FLAGS_BASE
#endif // Q_OS_MAC

// - Constructions -

SecondInputDialog::SecondInputDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  createLayout();
  timeEdit_->setFocus();

}

void
SecondInputDialog::createLayout()
{
  enum { TimeRange = 3600 * 3 }; // 3 houre
  timeEdit_ = new QtExt::SpinBox;
  timeEdit_->setMaximum(TimeRange);
  timeEdit_->setMinimum(-TimeRange);
  timeEdit_->setToolTip(TR(T_SECOND));
  timeEdit_->setFixedWidth(EDIT_WIDTH);
  timeEdit_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  connect(timeEdit_, SIGNAL(valueChanged(int)), SLOT(reportTime(int)));

  QToolButton *resetButton = AcUi::globalInstance()->makeToolButton(
        AcUi::PushHint, TR(T_RESET), this, SLOT(resetTime()));

  // Layouts
  QHBoxLayout *layout = new QHBoxLayout; {
    layout->addWidget(timeEdit_);
    layout->addWidget(resetButton);

    layout->setContentsMargins(9, 9, 9, 9);
    //setContentsMargins(0, 0, 0, 0);
  } setLayout(layout);
}

// - Properties -

qint64
SecondInputDialog::time() const
{ return timeEdit_->value() * 1000; }

void
SecondInputDialog::setTime(qint64 msecs)
{ timeEdit_->setValue(msecs / 1000); }

// EOF
