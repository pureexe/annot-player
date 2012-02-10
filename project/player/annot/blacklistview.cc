// blacklistview.cc
// 11/16/2011

#include "blacklistview.h"
#include "blacklistviewprivate.h"
#include "annotationfilter.h"
#include "uistyle.h"
#include "tr.h"
#include "defines.h"
#include "stylesheet.h"
#include "logger.h"
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

BlacklistView::BlacklistView(AnnotationFilter *filter, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), active_(false), tab_(TextTab), filter_(filter)
{
  Q_ASSERT(filter_);

  setWindowTitle(TR(T_TITLE_BLACKLIST));
  UiStyle::globalInstance()->setWindowStyle(this);
  setContentsMargins(0, 0, 0, 0);

  createActions();
  createTabs();
  createLayout();

  invalidateTab();

  QLineEdit *edit = currentLineEdit();
  if (edit)
    edit->setFocus();
}

bool
BlacklistView::active() const
{ return active_; }

void
BlacklistView::setActive(bool active)
{
  active_ = active;
  if (active_)
    connectFilter();
  else
    disconnectFilter();
}

#define CONNECT_FILTER(_connect) \
  void \
  BlacklistView::_connect##Filter() \
  { \
    _connect(filter_, SIGNAL(enabledChanged(bool)), this, SLOT(invalidateFilter())); \
    _connect(filter_, SIGNAL(blockedTextsChanged(QStringList)), textTab_, SLOT(setRows(QStringList))); \
    _connect(filter_, SIGNAL(blockedUserAliasesChanged(QStringList)), userTab_, SLOT(setRows(QStringList))); \
    _connect(filter_, SIGNAL(blockedAnnotationsChanged(AnnotationList)), annotationTab_, SLOT(setRows(AnnotationList))); \
\
    _connect(filter_, SIGNAL(blockedTextsChanged(QStringList)), this, SLOT(invalidateFilter())); \
    _connect(filter_, SIGNAL(blockedUserAliasesChanged(QStringList)), this, SLOT(invalidateFilter())); \
    _connect(filter_, SIGNAL(blockedAnnotationsChanged(AnnotationList)), this, SLOT(invalidateFilter())); \
  }

  CONNECT_FILTER(connect)
  CONNECT_FILTER(disconnect)
#undef CONNECT_FILTER

void
BlacklistView::createTabs()
{
  // Widgets
#define MAKE_TAB_BUTTON(_button, _tr, _slot) \
  _button = new QtExt::ToolButton; { \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
    _button->setText(QString("- %1 -").arg(_tr)); \
    _button->setCheckable(true); \
    connect(_button, SIGNAL(clicked()), _slot); \
  }

#define MAKE_UNCHECKABLE_BUTTON(_button, _title, _tip, _slot) \
    _button = new QtExt::ToolButton; { \
      _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
      _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
      _button->setText(_title); \
      _button->setToolTip(_tip); \
      connect(_button, SIGNAL(clicked()), _slot); \
    }
#define MAKE_CHECKABLE_BUTTON(_button, _title, _tip, _slot) \
    _button = new QtExt::ToolButton; { \
      _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
      _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
      _button->setText(_title); \
      _button->setToolTip(_tip); \
      _button->setCheckable(true); \
      connect(_button, SIGNAL(clicked(bool)), _slot); \
    }

  // User tab

  /*
  // - verbatimButton_
  MAKE_UNCHECKABLE_BUTTON(verbatimButton_, tr("verbatim"), TR(T_VERBATIM), SLOT(setVerbatim()))

  // - subtitleButton_
  MAKE_UNCHECKABLE_BUTTON(subtitleButton_, tr("subtitle"), TR(T_SUBTITLE), SLOT(setSubtitle()))

  // - moveStyleComboBox_
  moveStyleComboBox_ = new QtExt::ComboBox; {
    UiStyle::globalInstance()->setComboBoxStyle(moveStyleComboBox_);
    moveStyleComboBox_->setEditable(true);
    moveStyleComboBox_->setMaximumWidth(MOVESTYLECOMBOBOX_MAXWIDTH);
    moveStyleComboBox_->setToolTip(tr("Style tag"));

    // Must be consistent with MoveStyleIndex
    moveStyleComboBox_->addItem(tr("fly"));
    moveStyleComboBox_->addItem(tr("top"));
    moveStyleComboBox_->addItem(tr("bottom"));
  }
  connect(moveStyleComboBox_, SIGNAL(activated(int)), SLOT(setMoveStyle(int)));

  // - Code tab
  QGroupBox *codeTab = new QGroupBox; {
    QVBoxLayout *rows = new QVBoxLayout;
    QHBoxLayout *row0 = new QHBoxLayout,
                *row1 = new QHBoxLayout;
    rows->addLayout(row0);
    rows->addLayout(row1);
    codeTab->setLayout(rows);

    row0->addWidget(verbatimButton_);
    row0->addWidget(subtitleButton_);
    row0->addStretch();

    row1->addWidget(moveStyleComboBox_);
    row1->addStretch();

    int patch = 0;
    if (!UiStyle::isAeroAvailable())
      patch = 4;

    // void setContentsMargins(int left, int top, int right, int bottom);
    rows->setContentsMargins(patch, patch, patch, patch);
    row0->setContentsMargins(0, 0, 0, 0);
    row1->setContentsMargins(0, 0, 0, 0);
    codeTab->setContentsMargins(0, 0, 0, 0);
  }
  */

  // Tab layout

  textTab_ = new TextFilterView;
  userTab_ = new UserFilterView;
  annotationTab_ = new AnnotationFilterView;

  stackedLayout_ = new QStackedLayout; {
    // Must be consistent with enum Tab.
    stackedLayout_->addWidget(textTab_);
    stackedLayout_->addWidget(userTab_);
    stackedLayout_->addWidget(annotationTab_);
  }

  // Tabs

  MAKE_TAB_BUTTON(textTabButton_, TR(T_TEXT), SLOT(setTabToText()))
  MAKE_TAB_BUTTON(userTabButton_, TR(T_USER), SLOT(setTabToUser()))
  MAKE_TAB_BUTTON(annotationTabButton_, TR(T_ANNOTATION), SLOT(setTabToAnnotation()))

  // Footer

  MAKE_UNCHECKABLE_BUTTON(clearButton_, TR(T_CLEAR), TR(T_CLEAR), SLOT(clearCurrentText()));

  MAKE_UNCHECKABLE_BUTTON(addButton_, QString("[ %1 ]").arg(TR(T_ADD)), TR(T_ADD), SLOT(add()));
  MAKE_UNCHECKABLE_BUTTON(removeButton_, QString("[ %1 ]").arg(TR(T_REMOVE)), TR(T_REMOVE), SLOT(remove()));

  MAKE_CHECKABLE_BUTTON(enableButton_, TR(T_ENABLE), tr("Enable blacklist"), SLOT(setFilterEnabled(bool))) {
    enableButton_->setEnabled(true);
  }

#undef MAKE_TAB_BUTTON
#undef MAKE_UNCHECKABLE_BUTTON
#undef MAKE_CHECKABLE_BUTTON
}

void
BlacklistView::createLayout()
{
  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *footer = new QHBoxLayout;
    rows->addLayout(header);
    rows->addLayout(stackedLayout_);
    rows->addLayout(footer);

    header->addWidget(textTabButton_);
    header->addWidget(userTabButton_);
    if (ALPHA)
      header->addWidget(annotationTabButton_);
    else {
      annotationTabButton_->resize(0, 0);
      annotationTabButton_->setParent(this);
      annotationTabButton_->hide();
    }

    footer->addWidget(enableButton_);
    footer->addWidget(clearButton_);
    footer->addStretch();
    footer->addWidget(addButton_);
    footer->addWidget(removeButton_);

    int patch = 0;
    if (!UiStyle::isAeroAvailable())
      patch = 9;

    // left, top, right, bottom
    stackedLayout_->setContentsMargins(0, 0, 0, 0);
    header->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(9, patch, 9, patch);

  } setLayout(rows);
}

void
BlacklistView::createActions()
{
  // TODO - context menu
}

// - Slots -

void
BlacklistView::refresh()
{ invalidateFilter(); }

void
BlacklistView::invalidateFilter()
{
  bool enabled = filter_->isEnabled();

  enableButton_->setChecked(enabled);
  addButton_->setEnabled(enabled);
  clearButton_->setEnabled(enabled);
  removeButton_->setEnabled(enabled);
  textTab_->setEnabled(enabled);
  userTab_->setEnabled(enabled);
  annotationTab_->setEnabled(enabled);
  textTabButton_->setEnabled(enabled);
  userTabButton_->setEnabled(enabled);
  annotationTabButton_->setEnabled(enabled);

  textTab_->setRows(filter_->blockedTexts());
  userTab_->setRows(filter_->blockedUserAliases());
  annotationTab_->setRows(filter_->blockedAnnotations());
}

void
BlacklistView::setTabToText()
{ if (tab_ != TextTab) { tab_ = TextTab; invalidateTab(); } }

void
BlacklistView::setTabToUser()
{ if (tab_ != UserTab) { tab_ = UserTab; invalidateTab(); } }

void
BlacklistView::setTabToAnnotation()
{ if (tab_ != AnnotationTab) { tab_ = AnnotationTab; invalidateTab(); } }

void
BlacklistView::invalidateTab()
{
  stackedLayout_->setCurrentIndex(tab_);
  textTabButton_->setChecked(tab_ == TextTab);
  userTabButton_->setChecked(tab_ == UserTab);
  annotationTabButton_->setChecked(tab_ == AnnotationTab);
  addButton_->setEnabled(tab_ != AnnotationTab);
  clearButton_->setEnabled(tab_ != AnnotationTab);
}

QLineEdit*
BlacklistView::currentLineEdit()
{
  switch (tab_) {
  case TextTab: return textTab_->lineEdit();
  case UserTab: return userTab_->lineEdit();
  case AnnotationTab:   return 0;
  default: Q_ASSERT(0); return 0;
  }
}

void
BlacklistView::add()
{
  QLineEdit *edit = currentLineEdit();
  if (!edit)
    return;
  QString text = edit->text().trimmed();
  if (text.isEmpty())
    return;

  switch (tab_) {
  case TextTab: filter_->addBlockedText(text); break;
  case UserTab: filter_->addBlockedUserAlias(text); break;
  default: break;
  }
}

void
BlacklistView::clearCurrentText()
{
  QLineEdit *edit = currentLineEdit();
  if (edit)
    edit->clear();
}

void
BlacklistView::remove()
{
  switch (tab_) {
  case TextTab: {
      QString text = textTab_->current();
      if (!text.isEmpty()) {
        textTab_->removeCurrent();
        filter_->removeBlockedText(text);
      }
    } break;
  case UserTab: {
      QString text = userTab_->current();
      if (!text.isEmpty()) {
        userTab_->removeCurrent();
        filter_->removeBlockedUserAlias(text);
      }
    } break;
  case AnnotationTab: {
      qint64 id = annotationTab_->currentAnnotationId();
      if (id) {
        annotationTab_->removeCurrent();
        filter_->removeBlockedAnnotationWithId(id);
      }
    } break;
  default: Q_ASSERT(0); break;
  }
}

// - Events -

void
BlacklistView::setVisible(bool visible)
{
  if (visible)
    refresh();

  if (visible != isVisible()) {
    Base::setVisible(visible);
    setActive(visible);
  }
}

void
BlacklistView::setFilterEnabled(bool enabled)
{ filter_->setEnabled(enabled); }

// EOF
