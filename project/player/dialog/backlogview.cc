// backlogview.cc
// 1/1/2012

#include "backlogview.h"
#include "textview.h"
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

BacklogView::BacklogView(QWidget *parent)
  : Base(parent, WINDOW_FLAGS)
{
  setWindowTitle(TR(T_TITLE_BACKLOGVIEW));
  UiStyle::globalInstance()->setWindowStyle(this);

  createLayout();

  setAnnotationTab();
}

void
BacklogView::createLayout()
{
#define MAKE_TAB_BUTTON(_button, _text, _slot) \
  _button = new QtExt::ToolButton; { \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _button->setText(QString("- %1 -").arg(_text)); \
    _button->setToolTip(_text); \
    _button->setCheckable(true); \
    connect(_button, SIGNAL(clicked()), _slot); \
  }

#define MAKE_UNCHECKABLE_BUTTON(_button, _text, _slot) \
  _button = new QtExt::ToolButton; { \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _button->setText(QString("[ %1 ]").arg(_text)); \
    _button->setToolTip(_text); \
    connect(_button, SIGNAL(clicked()), _slot); \
  }

#define MAKE_CHECKABLE_BUTTON(_button, _text, _slot) \
  _button = new QtExt::ToolButton; { \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _button->setText(QString("[ %1 ]").arg(_text)); \
    _button->setToolTip(_text); \
    _button->setCheckable(true); \
    connect(_button, SIGNAL(clicked(bool)), _slot); \
  }

  MAKE_TAB_BUTTON(annotationButton_, TR(T_ANNOTATION), SLOT(setAnnotationTab()))
  MAKE_TAB_BUTTON(subtitleButton_, TR(T_SUBTITLE), SLOT(setSubtitleTab()))
  MAKE_TAB_BUTTON(textButton_, TR(T_TEXT), SLOT(setTextTab()))

  QToolButton*
  MAKE_UNCHECKABLE_BUTTON(clearButton, TR(T_CLEAR), SLOT(clear()))

  annotationView_ = new TextView;
  subtitleView_ = new TextView;
  textView_ = new TextView;

  stackLayout_ = new QStackedLayout; {
    // Must be consistent with TabIndex
    stackLayout_->addWidget(annotationView_);
    stackLayout_->addWidget(subtitleView_);
    stackLayout_->addWidget(textView_);
    stackLayout_->setContentsMargins(0, 0, 0, 0);
  }

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout;
    rows->addLayout(header);
    rows->addLayout(stackLayout_);

    header->addWidget(annotationButton_);
    header->addWidget(subtitleButton_);
    header->addWidget(textButton_);
    header->addStretch();
    header->addWidget(clearButton);

    // left, top, right, bottom
    int patch = 0;
    if (!UiStyle::isAeroAvailable())
      patch = 4;
    header->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(4, patch, 4, 4);

  } setLayout(rows);

#undef MAKE_TAB_BUTTON
#undef MAKE_UNCHECKABLE_BUTTON
#undef MAKE_CHECKABLE_BUTTON
}

// - Properties -

void
BacklogView::appendAnnotation(const QString &text)
{ annotationView_->append(text); }

void
BacklogView::appendSubtitle(const QString &text)
{ subtitleView_->append(text); }

void
BacklogView::appendText(const QString &text)
{ textView_->append(text); }

void
BacklogView::clear()
{
  annotationView_->clear();
  subtitleView_->clear();
  textView_->clear();
}

void
BacklogView::invalidateTabIndex()
{
  stackLayout_->setCurrentIndex(tabIndex_);
  annotationButton_->setChecked(tabIndex_ == AnnotationTabIndex);
  subtitleButton_->setChecked(tabIndex_ == SubtitleTabIndex);
  textButton_->setChecked(tabIndex_ == TextTabIndex);
}

// EOF
