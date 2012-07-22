// inputdialog.cc
// 2/8/2012

#include "inputdialog.h"
#include "tr.h"
#include "project/common/acui.h"
#include "project/common/accomboedit.h"
#include "module/qtext/datetime.h"
#include <QtGui>

#define DEBUG "inputdialog"
#include "module/debug/debug.h"

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
//#ifdef Q_OS_MAC
//  setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
//#endif // Q_OS_MAC
  setWindowTitle(tr("Input Dialog"));

  createLayout();

  // Focus
  edit_->setFocus();
}

void
InputDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  //QLabel *timeLabel = new QLabel;
  //timeLabel->setStyleSheet(SS_LABEL);
  //timeLabel->setText(TR(T_LABEL_SEEK));
  //timeLabel->setToolTip(TR(T_TOOLTIP_SEEK));

  edit_ = new AcComboEdit; {
    edit_->setToolTip(TR(T_TOOLTIP_INPUTLINE));
    edit_->setMaximumWidth(INPUTLINEEDIT_MAXWIDTH);
    edit_->lineEdit()->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  } connect(edit_->lineEdit(), SIGNAL(returnPressed()), SLOT(ok()));

  QToolButton *okButton =
      ui->makeToolButton(AcUi::PushHint, TR(T_OK), this, SLOT(ok()));

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    //QLayout *buttons = new QHBoxLayout;
    rows->addWidget(edit_);
    rows->addWidget(okButton);

    //buttons->addWidget(okButton);
    //buttons->addWidget(cancelButton);
    //rows->setContentsMargins(6, 6, 6, 6);
  } setLayout(rows);

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
  fadeOut();
  QString t = edit_->currentText().trimmed();
  if (!t.isEmpty())
    emit textEntered(t);
}

// EOF
