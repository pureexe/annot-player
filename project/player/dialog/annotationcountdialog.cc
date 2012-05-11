// annotationcountdialog.cc
// 2/8/2012

#include "annotationcountdialog.h"
#include "datamanager.h"
#include "tr.h"
#include "logger.h"
#include "project/common/acui.h"
#include "module/qtext/htmltag.h"
#include "module/qtext/datetime.h"
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

  createLayout();

  // Focus
  edit_->setFocus();
}

void
AnnotationCountDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  totalCountLabel_ = ui->makeLabel(
        AcUi::UrlHint, "", tr("Number of annotations"));
  QLabel *totalCountBuddy = ui->makeLabel(
        AcUi::BuddyHint, tr("Number of annotations"), totalCountLabel_);

  QStringList defvals = QStringList("0")
    << "100" << "500" << "1000" << "1500" << "3000" << "5000" << "8000" << "10000";
  edit_ = ui->makeComboBox(AcUi::EditHint, "", TR(T_TOOLTIP_INPUTLINE),  "", defvals);
  edit_->lineEdit()->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  connect(edit_->lineEdit(), SIGNAL(returnPressed()), SLOT(ok()));

  QToolButton *okButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_OK), this, SLOT(ok()));
  QToolButton *cancelButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CANCEL), this, SLOT(fadeOut()));

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
}

// - Actions -

void
AnnotationCountDialog::setCount(int count)
{ edit_->setEditText(QString::number(count)); }

void
AnnotationCountDialog::ok()
{
  fadeOut();
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
AnnotationCountDialog::updateTotalCount()
{
  totalCountLabel_->setText(
    QString::number(dm_->annotations().count())
  );
}

void
AnnotationCountDialog::setVisible(bool visible)
{
  if (visible)
    updateTotalCount();
  Base::setVisible(visible);
}

// EOF
