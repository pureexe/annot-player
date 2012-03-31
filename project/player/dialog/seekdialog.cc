// seekdialog.cc
// 8/16/2011

#include "seekdialog.h"
#include "tr.h"
#include "global.h"
#include "logger.h"
#include "ac/acui.h"
#include "module/qtext/datetime.h"
#include <QtGui>

#define DEBUG "seekdialog"
#include "module/debug/debug.h"

using namespace Logger;

#define SEEKLINEEDIT_MAXWIDTH         50

#define BAD_TIME    -1

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint )

// - Constructions -

SeekDialog::SeekDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
//#ifdef Q_WS_MAC
//  setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
//#endif // Q_WS_MAC
  setWindowTitle(TR(T_TITLE_SEEK));

  createLayout();

  // Focus
  ssEdit_->setFocus();
}

void
SeekDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);

  QStringList defvals = QStringList()
    << "0" << "1" << "2" << "3" << "5"
    << "10" << "20" << "30" << "40" << "50";

  ssEdit_ = ui->makeComboBox(AcUi::EditHint, "", TR(T_SECOND), defvals);
  ssEdit_->lineEdit()->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ssEdit_->setMaximumWidth(SEEKLINEEDIT_MAXWIDTH);

  mmEdit_ = ui->makeComboBox(AcUi::EditHint, "", TR(T_MINUTE), defvals);
  mmEdit_->lineEdit()->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  mmEdit_->setMaximumWidth(SEEKLINEEDIT_MAXWIDTH);

  hhEdit_ = ui->makeComboBox(AcUi::EditHint, "", TR(T_HOUR), defvals);
  hhEdit_->lineEdit()->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  hhEdit_->setMaximumWidth(SEEKLINEEDIT_MAXWIDTH);

  QLabel *ssLabel = ui->makeLabel(AcUi::BuddyHint, tr("ss"), tr("seconds"), ssEdit_),
         *mmLabel = ui->makeLabel(AcUi::BuddyHint, tr("mm"), tr("minutes"), mmEdit_),
         *hhLabel = ui->makeLabel(AcUi::BuddyHint, tr("hh"), tr("hours"), hhEdit_);

  QToolButton *okButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_OK), this, SLOT(ok()));
  QToolButton *cancelButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CANCEL), this, SLOT(hide()));

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    QLayout *row1 = new QHBoxLayout,
            *row2 = new QHBoxLayout;
    rows->addLayout(row1);
    rows->addLayout(row2);

    //row1->addWidget(timeLabel);
    row1->addWidget(hhEdit_);
    row1->addWidget(hhLabel);
    row1->addWidget(mmEdit_);
    row1->addWidget(mmLabel);
    row1->addWidget(ssEdit_);
    row1->addWidget(ssLabel);

    row2->addWidget(cancelButton);
    row2->addWidget(okButton);
    rows->setContentsMargins(6, 6, 6, 6);
  } setLayout(rows);

  setTabOrder(hhEdit_, mmEdit_);
  setTabOrder(mmEdit_, ssEdit_);
  setTabOrder(ssEdit_, hhEdit_);

  // Connections
  connect(ssEdit_->lineEdit(), SIGNAL(returnPressed()), SLOT(ok()));
  connect(mmEdit_->lineEdit(), SIGNAL(returnPressed()), SLOT(ok()));
  connect(hhEdit_->lineEdit(), SIGNAL(returnPressed()), SLOT(ok()));

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));
}

// - Properties -

qint64
SeekDialog::time() const
{
  bool ok;
  qint64 ss = ssEdit_->currentText().toULong(&ok);
  if (!ok) return BAD_TIME;

  qint64 mm = mmEdit_->currentText().toULong(&ok);
  if (!ok) return BAD_TIME;

  qint64 hh = hhEdit_->currentText().toULong(&ok);
  if (!ok) return BAD_TIME;

  DOUT("time: hh =" << hh << ", mm =" << mm << ", ss =" << ss);
  return ss * 1000 + mm * 1000 * 60 + hh * 1000 * 60 * 60;
}

void
SeekDialog::setTime(qint64 msecs)
{
  QTime t = QtExt::msecs2time(msecs);
  if (!t.isValid())
    return;

  int hh = t.hour(),
      mm = t.minute(),
      ss = t.second();

  ssEdit_->setEditText(QString::number(ss));
  mmEdit_->setEditText(QString::number(mm));
  hhEdit_->setEditText(QString::number(hh));
}

// - Slots -

void
SeekDialog::ok()
{
  qint64 msecs = time();
  if (msecs < 0) {
    warn(TR(T_ERROR_BAD_POS));
    return;
  }
  hide();
  emit seekRequested(msecs);
}

// EOF
