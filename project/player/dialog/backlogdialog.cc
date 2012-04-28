// backlogdialog.cc
// 1/1/2012

#include "backlogdialog.h"
#include "textedittabview.h"
#include "tr.h"
#include "project/common/acui.h"
#include <QtGui>

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

BacklogDialog::BacklogDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(TR(T_TITLE_BACKLOG));

  createLayout();

  // Shortcuts
  connect(new QShortcut(QKeySequence("Esc"), this), SIGNAL(activated()), SLOT(hide()));
  connect(new QShortcut(QKeySequence("CTRL+W"), this), SIGNAL(activated()), SLOT(hide()));
}

void
BacklogDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);

  tabView_ = new TextEditTabView;
  tabView_->addTab(tr("Annot"));
  tabView_->addTab(tr("Subtitle"));
  tabView_->addTab(tr("Text"));
  tabView_->finalizeLayout();

  QToolButton *okButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_OK), this, SLOT(hide()));
  QToolButton *clearButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CLEAR), this, SLOT(clear()));

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *footer = new QHBoxLayout;
    rows->addWidget(tabView_);
    rows->addLayout(footer);

    footer->addWidget(clearButton);
    footer->addWidget(okButton);

    // left, top, right, bottom
    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 9;
    footer->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(9, patch, 9, 9);
    setContentsMargins(0, 0, 0, 0);
  } setLayout(rows);
}

// - Properties -

void
BacklogDialog::appendAnnotation(const QString &text)
{ tabView_->appendText(text, AnnotationTabIndex); }

void
BacklogDialog::appendSubtitle(const QString &text)
{ tabView_->appendText(text, SubtitleTabIndex); }

void
BacklogDialog::appendText(const QString &text)
{ tabView_->appendText(text, TextTabIndex); }

void
BacklogDialog::clear()
{
  for (int i = 0; i < TabIndexCount; i++)
    tabView_->clear(i);
}

// EOF
