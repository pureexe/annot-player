// inputdialog.cc
// 2/8/2012

#include "inputdialog.h"
#include "uistyle.h"
#include "tr.h"
#include "defines.h"
#include "stylesheet.h"
#include "comboedit.h"
#include "logger.h"
#include "module/qtext/datetime.h"
#include "module/qtext/toolbutton.h"
#include <QtGui>

#define DEBUG "inputdialog"
#include "module/debug/debug.h"

using namespace Logger;

#define INPUTLINEEDIT_MAXWIDTH         100

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint )

// - Constructions -

InputDialog::InputDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
//#ifdef Q_WS_MAC
//  setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
//#endif // Q_WS_MAC
  setWindowTitle(tr("Input Dialog"));
  UiStyle::globalInstance()->setWindowStyle(this);

  // Widgets

  //QLabel *timeLabel = new QLabel;
  //timeLabel->setStyleSheet(SS_LABEL);
  //timeLabel->setText(TR(T_LABEL_SEEK));
  //timeLabel->setToolTip(TR(T_TOOLTIP_SEEK));

  edit_ = new ComboEdit; {
    edit_->setToolTip(TR(T_TOOLTIP_INPUTLINE));
    edit_->setMaximumWidth(INPUTLINEEDIT_MAXWIDTH);
    edit_->lineEdit()->setAlignment(Qt::AlignRight);
  } connect(edit_->lineEdit(), SIGNAL(returnPressed()), SLOT(ok()));

#define MAKE_BUTTON(_button, _text, _tip, _slot) \
  QToolButton *_button = new QtExt::ToolButton; { \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _button->setText(_text); \
    _button->setToolTip(_tip); \
  } \
  connect(_button, SIGNAL(clicked()), _slot);

  MAKE_BUTTON(okButton, QString("[ %1 ]").arg(TR(T_OK)), TR(T_OK), SLOT(ok()))
  //MAKE_BUTTON(cancelButton, QString("[ %1 ]").arg(TR(T_CANCEL)), TR(T_CANCEL), SLOT(hide()))
#undef MAKE_BUTTON

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    //QLayout *buttons = new QHBoxLayout;
    rows->addWidget(edit_);
    rows->addWidget(okButton);

    //buttons->addWidget(okButton);
    //buttons->addWidget(cancelButton);
    //rows->setContentsMargins(6, 6, 6, 6);
  } setLayout(rows);

  // Focus
  edit_->setFocus();
}

// - Properties -

void
InputDialog::setText(const QString &t)
{ edit_->setEditText(t); }

void
InputDialog::setToolTip(const QString &t)
{ edit_->setToolTip(t); }

void
InputDialog::setDefaultItems(const QStringList &l)
{ edit_->setDefaultItems(l); }

// - Actions -

void
InputDialog::ok()
{
  hide();
  QString t = edit_->currentText().trimmed();
  if (!t.isEmpty())
    emit textEntered(t);
}

// EOF
