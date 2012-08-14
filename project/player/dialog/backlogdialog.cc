// backlogdialog.cc
// 1/1/2012

#include "backlogdialog.h"
#include "textedittabview.h"
#include "textview.h"
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

  foreach (QWidget *w, tabView_->widgets())
    connect(qobject_cast<TextView *>(w), SIGNAL(selectedTextChanged(QString)), SIGNAL(translateRequested(QString)));
}

void
BacklogDialog::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  tabView_ = new TextEditTabView(this);
  tabView_->addTab(tr("Comment"));
  tabView_->addTab(tr("Subtitle"));
#ifdef BACKLOGDIALOG_HAS_TEXT_TAB
  tabView_->addTab(tr("Text"));
#endif // BACKLOGDIALOG_HAS_TEXT_TAB
  tabView_->finalizeLayout();

  QToolButton *translateButton = ui->makeToolButton(
        AcUi::CheckHint, tr("Translate"), tr("Automatically translate seletected text"), this, SLOT(setTranslateEnabled(bool)));
  QToolButton *clearButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CLEAR), this, SLOT(clear()));

  translateButton->click(); // enable translation by default

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *footer = new QHBoxLayout;
    rows->addWidget(tabView_);
    rows->addLayout(footer);

    footer->addWidget(clearButton);
    footer->addWidget(translateButton);

    // left, top, right, bottom
    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 9;
    footer->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(patch, patch, patch, patch);
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

#ifdef BACKLOGDIALOG_HAS_TEXT_TAB
void
BacklogDialog::appendText(const QString &text)
{
  tabView_->appendText(text, TextTabIndex);
  appendSubtitle(text);
}
#endif // BACKLOGDIALOG_HAS_TEXT_TAB

// - Actions -

void
BacklogDialog::setTranslateEnabled(bool t)
{
  foreach (QWidget *w, tabView_->widgets())
    qobject_cast<TextView *>(w)->setSelectionMonitored(t);
}

void
BacklogDialog::clear()
{
  for (int i = 0; i < TabIndexCount; i++)
    tabView_->clear(i);
}

// EOF
