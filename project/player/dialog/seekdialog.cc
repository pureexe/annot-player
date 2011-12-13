// seekdialog.cc
// 8/16/2011

#include "seekdialog.h"
#include "uistyle.h"
#include "tr.h"
#include "global.h"
#include "stylesheet.h"
#include "lineedit.h"
#include "logger.h"
#include "core/util/datetime.h"
#include "core/gui/toolbutton.h"
#include <QtGui>

#define DEBUG "seekdialog"
#include "module/debug/debug.h"

using namespace Logger;

#define SEEKLINEEDIT_MAXWIDTH         40

#define BAD_TIME    -1

// - Constructions -

SeekDialog::SeekDialog(QWidget *parent)
  : Base(parent)
{
#ifdef Q_WS_MAC
  setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
#endif // Q_WS_MAC
  setWindowTitle(TR(T_TITLE_SEEK));
  UiStyle::globalInstance()->setWindowStyle(this);
  setContentsMargins(0, 0, 0, 0);

  // Widgets

  //QLabel *timeLabel = new QLabel;
  //timeLabel->setStyleSheet(SS_LABEL);
  //timeLabel->setText(TR(T_LABEL_SEEK));
  //timeLabel->setToolTip(TR(T_TOOLTIP_SEEK));

#define MAKE_EDIT(_edit) \
  _edit = new LineEdit; { \
    _edit->setStyleSheet(SS_LINEEDIT); \
    _edit->setAlignment(Qt::AlignRight); \
    _edit->setToolTip(TR(T_TOOLTIP_SEEK)); \
    _edit->setText("0"); \
    _edit->setMaximumWidth(SEEKLINEEDIT_MAXWIDTH); \
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

#define MAKE_BUTTON(_button, _text, _tip) \
  QToolButton *_button = new Core::Gui::ToolButton; { \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _button->setText(_text); \
    _button->setToolTip(_tip); \
  }

  MAKE_BUTTON(okButton, QString("[ %1 ]").arg(TR(T_OK)), TR(T_OK))
  MAKE_BUTTON(cancelButton, QString("[ %1 ]").arg(TR(T_CANCEL)), TR(T_CANCEL))
#undef MAKE_BUTTON

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *row1 = new QHBoxLayout,
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

    row2->addWidget(okButton);
    row2->addWidget(cancelButton);
  } setLayout(rows);

  setTabOrder(ssEdit_, mmEdit_);
  setTabOrder(mmEdit_, hhEdit_);
  setTabOrder(hhEdit_, ssEdit_);

  // Connections
  connect(cancelButton, SIGNAL(clicked()), SLOT(cancel()));
  connect(okButton, SIGNAL(clicked()), SLOT(ok()));
  connect(ssEdit_, SIGNAL(returnPressed()), SLOT(ok()));
  connect(mmEdit_, SIGNAL(returnPressed()), SLOT(ok()));
  connect(hhEdit_, SIGNAL(returnPressed()), SLOT(ok()));

  // Focus
  ssEdit_->setFocus();
}

// - Properties -

qint64
SeekDialog::time() const
{
  bool ok;
  ulong ss = ssEdit_->text().toULong(&ok);
  if (!ok) return BAD_TIME;

  ulong mm = mmEdit_->text().toULong(&ok);
  if (!ok) return BAD_TIME;

  ulong hh = hhEdit_->text().toULong(&ok);
  if (!ok) return BAD_TIME;

  DOUT("time: hh =" << hh << ", mm =" << mm << ", ss =" << ss);
  return static_cast<qint64>(ss * 1000 + mm * 1000 * 60 + hh * 1000 * 60 * 60);
}

void
SeekDialog::setTime(qint64 msecs)
{
  QTime t = Core::msecs2time(msecs);
  if (!t.isValid())
    return;

  int hh = t.hour(),
      mm = t.minute(),
      ss = t.second();

  ssEdit_->setText(QString::number(ss));
  mmEdit_->setText(QString::number(mm));
  hhEdit_->setText(QString::number(hh));
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

void
SeekDialog::cancel()
{ hide(); }

// EOF
