// blacklistview.cc
// 11/16/2011

#include "blacklistview.h"
#include "blacklistview_p.h"
#include "annotationfilter.h"
#include "global.h"
#include "tr.h"
#include "project/common/acui.h"
#include <QtGui>

#ifdef Q_OS_MAC
# define K_CTRL        "cmd"
#else
# define K_CTRL        "Ctrl"
#endif // Q_OS_MAC

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

  createTabs();
  createLayout();
  createActions();

  invalidateTab();

  QLineEdit *edit = currentLineEdit();
  if (edit)
    edit->setFocus();
}

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
  AcUi *ui = AcUi::globalInstance();

  // Tab layout

  textTab_ = new TextFilterView(this);
  userTab_ = new UserFilterView(this);
  annotationTab_ = new AnnotationFilterView(this);

  stackedLayout_ = new QStackedLayout; {
    // Must be consistent with enum Tab.
    stackedLayout_->addWidget(textTab_);
    stackedLayout_->addWidget(userTab_);
    stackedLayout_->addWidget(annotationTab_);
  }

  // Tabs

  textTabButton_ = ui->makeToolButton(AcUi::TabHint, TR(T_TEXT), "", K_CTRL "+1", this, SLOT(setTabToText()));
  userTabButton_ = ui->makeToolButton(AcUi::TabHint, TR(T_USER), "", K_CTRL "+2", this, SLOT(setTabToUser()));
  annotationTabButton_ = ui->makeToolButton(AcUi::TabHint, TR(T_ANNOTATION), "", K_CTRL "+3", this, SLOT(setTabToAnnotation()));

  // Footer

  addButton_ = ui->makeToolButton(AcUi::PushHint | AcUi::HighlightHint, TR(T_ADD), this, SLOT(add()));
  clearButton_ = ui->makeToolButton(AcUi::PushHint, TR(T_CLEAR), this, SLOT(clearCurrentText()));
  removeButton_ = ui->makeToolButton(AcUi::PushHint, TR(T_REMOVE), this, SLOT(remove()));

  enableButton_ = ui->makeToolButton(
        AcUi::CheckHint, TR(T_ENABLE), tr("Enable blacklist"), this, SLOT(setFilterEnabled(bool)));
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
    if (!AcUi::isAeroAvailable())
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
  new QShortcut(QKeySequence("CTRL+1"), textTabButton_, SLOT(click()));
  new QShortcut(QKeySequence("CTRL+2"), userTabButton_, SLOT(click()));
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
  default: ;
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
  default: Q_ASSERT(0);
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
