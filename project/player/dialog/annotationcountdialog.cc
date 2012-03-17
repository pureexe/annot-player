// annotationcountdialog.cc
// 2/8/2012

#include "annotationcountdialog.h"
#include "datamanager.h"
#include "uistyle.h"
#include "tr.h"
#include "defines.h"
#include "stylesheet.h"
#include "comboedit.h"
#include "logger.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/datetime.h"
#include "module/qtext/toolbutton.h"
#include <QtGui>

#define DEBUG "annotationcountdialog"
#include "module/debug/debug.h"

using namespace Logger;

//#define INPUTLINEEDIT_MAXWIDTH         100

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint )

// - Constructions -

AnnotationCountDialog::AnnotationCountDialog(DataManager *dm, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), dm_(dm)
{
  Q_ASSERT(dm_);
//#ifdef Q_WS_MAC
//  setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
//#endif // Q_WS_MAC
  setWindowTitle(tr("Annot Max Count Hint"));
  setToolTip(tr("Maximum numbers of annotations to display"));
  UiStyle::globalInstance()->setWindowStyle(this);

  // Widgets

  totalCountLabel_ = new QLabel; {
    totalCountLabel_->setStyleSheet(SS_LABEL_HIGHLIGHT);
    totalCountLabel_->setToolTip(tr("Number of annotations"));
  }
  QLabel *totalCountBuddy = new QLabel; {
    totalCountBuddy->setStyleSheet(SS_LABEL);
    totalCountBuddy->setBuddy(totalCountLabel_);
    totalCountBuddy->setText(tr("Number of annotations") + ":");
    totalCountBuddy->setToolTip(tr("Number of annotations"));
  }

  QStringList defvals = QStringList("0")
    << "100" << "500" << "1000" << "1500" << "3000" << "5000" << "8000" << "10000";
  edit_ = new ComboEdit(defvals); {
    edit_->setToolTip(TR(T_TOOLTIP_INPUTLINE));
    //edit_->setMaximumWidth(INPUTLINEEDIT_MAXWIDTH);
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
  MAKE_BUTTON(cancelButton, QString("[ %1 ]").arg(TR(T_CANCEL)), TR(T_CANCEL), SLOT(hide()))
#undef MAKE_BUTTON
  okButton->setStyleSheet(SS_TOOLBUTTON_TEXT_HIGHLIGHT);

  // Layouts
  QVBoxLayout *rows = new QVBoxLayout; {
    QLayout *header = new QHBoxLayout,
            *footer = new QHBoxLayout;
    rows->addLayout(header);
    rows->addWidget(edit_);
    rows->addLayout(footer);

    header->addWidget(totalCountBuddy);
    header->addWidget(totalCountLabel_);

    footer->addWidget(cancelButton);
    footer->addWidget(okButton);
    //rows->setContentsMargins(6, 6, 6, 6);
  } setLayout(rows);

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));

  // Focus
  edit_->setFocus();
}

// - Actions -

void
AnnotationCountDialog::setCount(int count)
{ edit_->setEditText(QString::number(count)); }

void
AnnotationCountDialog::ok()
{
  hide();
  uint count = edit_->currentText().toUInt();
  if (count)
    log(QString("%1: " HTML_STYLE_OPEN(color:red) " %2" HTML_STYLE_CLOSE())
        .arg(tr("annotation max count"))
        .arg(QString::number(count)));
  else
    log(tr("annotation maximum count disabled"));
  emit countChanged(count);
}

void
AnnotationCountDialog::invalidateTotalCount()
{
  totalCountLabel_->setText(
    QString::number(dm_->annotations().count())
  );
}

void
AnnotationCountDialog::setVisible(bool visible)
{
  if (visible)
    invalidateTotalCount();
  Base::setVisible(visible);
}

// EOF
