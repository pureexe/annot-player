// backlogdialog.cc
// 1/1/2012

#include "backlogdialog.h"
#include "textedittabview.h"
#include "tr.h"
#include "uistyle.h"
#include "stylesheet.h"
#include "module/qtext/toolbutton.h"
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
}

void
BacklogDialog::createLayout()
{
  tabView_ = new TextEditTabView;
  tabView_->addTab(tr("Annot"));
  tabView_->addTab(tr("Subtitle"));
  tabView_->addTab(tr("Text"));
  tabView_->finalizeLayout();

#define MAKE_BUTTON(_button, _text, _slot) \
  QToolButton *_button = new QtExt::ToolButton; { \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _button->setText(QString("[ %1 ]").arg(_text)); \
    _button->setToolTip(_text); \
    connect(_button, SIGNAL(clicked()), _slot); \
  }

  MAKE_BUTTON(okButton, TR(T_OK), SLOT(hide()))
  MAKE_BUTTON(clearButton, TR(T_CLEAR), SLOT(clear()))

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

#undef MAKE_BUTTON
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
