// seekdialog.cc
// 8/16/2011

#include "seekdialog.h"
#include "uistyle.h"
#include "tr.h"
#include "defines.h"
#include "stylesheet.h"
#include "comboedit.h"
#include "logger.h"
#include "module/qtext/datetime.h"
#include "module/qtext/toolbutton.h"
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
  UiStyle::globalInstance()->setWindowStyle(this);

  // Widgets

  //QLabel *timeLabel = new QLabel;
  //timeLabel->setStyleSheet(SS_LABEL);
  //timeLabel->setText(TR(T_LABEL_SEEK));
  //timeLabel->setToolTip(TR(T_TOOLTIP_SEEK));

  QStringList defvals = QStringList()
    << "0" << "1" << "2" << "3" << "5"
    << "10" << "20" << "30" << "40" << "50";

#define MAKE_EDIT(_edit) \
  _edit = new ComboEdit(defvals); { \
    _edit->setToolTip(TR(T_TOOLTIP_SEEK)); \
    _edit->setEditText("0"); \
    _edit->setMaximumWidth(SEEKLINEEDIT_MAXWIDTH); \
    _edit->lineEdit()->setAlignment(Qt::AlignRight); \
  }

  MAKE_EDIT(ssEdit_)
  MAKE_EDIT(mmEdit_)
  MAKE_EDIT(hhEdit_)
#undef MAKE_EDIT

#define MAKE_LABEL(_label, _buddy, _text, _tip) \
  QLabel *_label = new QLabel; { \
    _label->setBuddy(_buddy); \
    _label->setStyleSheet(SS_LABEL); \
    _label->setText(_text); \
    _label->setToolTip(_tip); \
  }

  MAKE_LABEL(ssLabel, ssEdit_, tr("ss"), tr("seconds"))
  MAKE_LABEL(mmLabel, mmEdit_, tr("mm"), tr("minutes"))
  MAKE_LABEL(hhLabel, hhEdit_, tr("hh"), tr("hours"))
#undef MAKE_LABEL

#define MAKE_BUTTON(_button, _text, _tip, _slot) \
  QToolButton *_button = new QtExt::ToolButton; { \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _button->setText(_text); \
    _button->setToolTip(_tip); \
  } \
  connect(_button, SIGNAL(clicked()), _slot);

  MAKE_BUTTON(okButton, QString("[ %1 ]").arg(TR(T_OK)), TR(T_OK), SLOT(ok()))
  MAKE_BUTTON(cancelButton, QString("[ %1 ]").arg(TR(T_CANCEL)), TR(T_CANCEL), SLOT(hide()))
#undef MAKE_BUTTON

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

  // Focus
  ssEdit_->setFocus();
}

// - Properties -

qint64
SeekDialog::time() const
{
  bool ok;
  ulong ss = ssEdit_->currentText().toULong(&ok);
  if (!ok) return BAD_TIME;

  ulong mm = mmEdit_->currentText().toULong(&ok);
  if (!ok) return BAD_TIME;

  ulong hh = hhEdit_->currentText().toULong(&ok);
  if (!ok) return BAD_TIME;

  DOUT("time: hh =" << hh << ", mm =" << mm << ", ss =" << ss);
  return static_cast<qint64>(ss * 1000 + mm * 1000 * 60 + hh * 1000 * 60 * 60);
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
