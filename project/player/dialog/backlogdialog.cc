// backlogdialog.cc
// 1/1/2012

#include "backlogdialog.h"
#include "textedittabview.h"
#include "tr.h"
#include "uistyle.h"
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
  UiStyle::globalInstance()->setWindowStyle(this);

  createLayout();

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));
}

void
BacklogDialog::createLayout()
{
  tabView_ = new TextEditTabView;
  tabView_->addTab(tr("Annot"));
  tabView_->addTab(tr("Subtitle"));
  tabView_->addTab(tr("Text"));
  tabView_->finalizeLayout();

  QToolButton *okButton = UiStyle::globalInstance()->makeToolButton(
        UiStyle::PushHint | UiStyle::HighlightHint, TR(T_OK), this, SLOT(hide()));
  QToolButton *clearButton = UiStyle::globalInstance()->makeToolButton(
        UiStyle::PushHint, TR(T_CLEAR), this, SLOT(clear()));

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *footer = new QHBoxLayout;
    rows->addWidget(tabView_);
    rows->addLayout(footer);

    footer->addWidget(clearButton);
    footer->addWidget(okButton);

    // left, top, right, bottom
    int patch = 0;
    if (!UiStyle::isAeroAvailable())
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
