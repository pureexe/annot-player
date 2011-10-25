// tokenview.cc
// 8/13/2011

#include "tokenview.h"
#include "lineedit.h"
#include "addaliasdialog.h"
#include "tr.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "core/gui/combobox.h"
#include "core/gui/toolbutton.h"
#include <QtGui>

using namespace Core::Data;

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

TokenView::TokenView(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), userId_(0)
{
  setWindowTitle(TR(T_TITLE_TOKENVIEW));
  UiStyle::globalInstance()->setWindowStyle(this);

  // Create models

  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setAliasHeaderData(sourceModel_);

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(sourceModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  }

  // Create widgets
  aliasDialog_ = new AddAliasDialog(this);
  connect(aliasDialog_, SIGNAL(aliasAdded(QString,int,quint32)), SLOT(submitAlias(QString,int,quint32)));

  proxyView_ = new QTreeView; {
    proxyView_->setStyleSheet(SS_TREEVIEW);
    proxyView_->setRootIsDecorated(true);
    proxyView_->setAlternatingRowColors(true);
    proxyView_->setModel(proxyModel_);
    proxyView_->setSortingEnabled(true);
    proxyView_->setToolTip(tr("Token information"));
  }

  filterPatternLineEdit_ = new LineEdit; {
    filterPatternLineEdit_->setStyleSheet(SS_LINEEDIT);
    filterPatternLineEdit_->setToolTip(tr("Filter pattern"));
  }
  QLabel *filterPatternLabel = new QLabel; {
    filterPatternLabel->setStyleSheet(SS_LABEL);
    filterPatternLabel->setBuddy(filterPatternLineEdit_);
    filterPatternLabel->setText(tr("&Filter pattern") + ":");
    filterPatternLabel->setToolTip(filterPatternLineEdit_->toolTip());
  }

  filterSyntaxComboBox_ = new Core::Gui::ComboBox; {
    UiStyle::globalInstance()->setComboBoxStyle(filterSyntaxComboBox_);
    filterSyntaxComboBox_->addItem(tr("Regular expression"), QRegExp::RegExp);
    filterSyntaxComboBox_->addItem(tr("Wildcard"), QRegExp::Wildcard);
    filterSyntaxComboBox_->addItem(tr("Fixed string"), QRegExp::FixedString);
    filterSyntaxComboBox_->setToolTip(tr("Filter syntax"));
  }
  QLabel *filterSyntaxLabel = new QLabel; {
    filterSyntaxLabel->setStyleSheet(SS_LABEL);
    filterSyntaxLabel->setBuddy(filterSyntaxComboBox_);
    filterSyntaxLabel->setText(tr("Filter &syntax") + ":");
    filterSyntaxLabel->setToolTip(filterSyntaxComboBox_->toolTip());
  }

  filterColumnComboBox_ = new Core::Gui::ComboBox; {
    UiStyle::globalInstance()->setComboBoxStyle(filterColumnComboBox_);
    for (int i = 0; i < sourceModel_->columnCount(); i++)
      filterColumnComboBox_->addItem(sourceModel_->headerData(i, Qt::Horizontal).toString());
    filterColumnComboBox_->setToolTip(tr("Filter column"));
  }
  QLabel *filterColumnLabel = new QLabel; {
    filterColumnLabel->setStyleSheet(SS_LABEL);
    filterColumnLabel->setBuddy(filterColumnComboBox_);
    filterColumnLabel->setText(tr("Filter &column"));
    filterColumnLabel->setToolTip(filterColumnComboBox_->toolTip());
  }

#define MAKE_TOKEN_LABEL(_id, _styleid) \
  _id##Label_ = new QLabel; { \
    _id##Label_->setStyleSheet(SS_LABEL); \
    _id##Label_->setText(TR(T_UNKNOWN)); \
    _id##Label_->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  } \
  QLabel *_id##Buddy = new QLabel; { \
    _id##Buddy->setStyleSheet(SS_LABEL); \
    _id##Buddy->setBuddy(_id##Label_); \
    _id##Buddy->setText(TR(T_LABEL_##_styleid)); \
    _id##Buddy->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  }

  MAKE_TOKEN_LABEL(createDate, CREATEDATE)
  MAKE_TOKEN_LABEL(blessedCount, BLESSEDCOUNT)
  MAKE_TOKEN_LABEL(cursedCount, CURSEDCOUNT)
  MAKE_TOKEN_LABEL(visitedCount, VISITEDCOUNT)
#undef MAKE_TOKEN_LABEL

#define MAKE_BUTTON(_button, _text, _tip, _slot) \
  _button = new Core::Gui::ToolButton; { \
    _button->setText(QString("[ %1 ]").arg(_text)); \
    _button->setToolTip(_tip); \
    _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
    _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
  } \
  connect(_button, SIGNAL(clicked()), _slot);

  QToolButton *MAKE_BUTTON(blessButton, TR(T_BLESS), TR(T_TOOLTIP_BLESSTHISTOKEN), SLOT(bless()))
  QToolButton *MAKE_BUTTON(curseButton, TR(T_CURSE), TR(T_TOOLTIP_CURSETHISTOKEN), SLOT(curse()))
  QToolButton *MAKE_BUTTON(addAliasButton, TR(T_ADD), TR(T_TOOLTIP_CURSETHISTOKEN), SLOT(addAlias()))
#undef MAKE_BUTTON

  QLabel *aliasBuddy = new QLabel; {
    aliasBuddy->setStyleSheet(SS_LABEL);
    aliasBuddy->setBuddy(addAliasButton);
    aliasBuddy->setText(TR(T_LABEL_ALIAS));
    aliasBuddy->setToolTip(TR(T_TOOLTIP_ALIAS));
  }

  // Set layout

  QGridLayout *layout = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    layout->addWidget(createDateBuddy, r=0, c=0);
    layout->addWidget(createDateLabel_, r, ++c);

    layout->addWidget(visitedCountBuddy, ++r, c=0);
    layout->addWidget(visitedCountLabel_, r, ++c);

    layout->addWidget(blessedCountBuddy, ++r, c=0);
    layout->addWidget(blessedCountLabel_, r, ++c);
    layout->addWidget(blessButton, r, ++c);

    layout->addWidget(cursedCountBuddy, ++r, c=0);
    layout->addWidget(cursedCountLabel_, r, ++c);
    layout->addWidget(curseButton, r, ++c);

    layout->addWidget(aliasBuddy, ++r, c=0);
    ++c;
    layout->addWidget(addAliasButton, r, ++c);

    layout->addWidget(proxyView_, ++r, c=0, 1, 3);

    layout->addWidget(filterPatternLabel, ++r, c=0);
    layout->addWidget(filterPatternLineEdit_, r, ++c, 1, 2);

    layout->addWidget(filterSyntaxLabel, ++r, c=0);
    layout->addWidget(filterSyntaxComboBox_, r, ++c, 1, 2);

    layout->addWidget(filterColumnLabel, ++r, c=0);
    layout->addWidget(filterColumnComboBox_, r, ++c, 1, 2);

    //layout->setContentsMargins(0, 0, 0, 0);
  }
  setLayout(layout);

  // Set up connections
  connect(filterPatternLineEdit_, SIGNAL(textChanged(QString)),
          SLOT(invalidateFilterRegExp()));
  connect(filterSyntaxComboBox_, SIGNAL(currentIndexChanged(int)),
          SLOT(invalidateFilterRegExp()));
  connect(filterColumnComboBox_, SIGNAL(currentIndexChanged(int)),
          SLOT(invalidateFilterColumn()));

  // Set initial states

  proxyView_->sortByColumn(HD_Text, Qt::AscendingOrder);
  filterColumnComboBox_->setCurrentIndex(HD_Text);

  // Focus:
  filterPatternLineEdit_->setFocus();
}

qint64
TokenView::userId() const
{ return userId_; }

void
TokenView::setUserId(qint64 uid)
{ userId_ = uid; }

void
TokenView::setAliasHeaderData(QAbstractItemModel *model)
{
  model->setHeaderData(HD_Type, Qt::Horizontal, tr("Type"));
  model->setHeaderData(HD_Language, Qt::Horizontal, tr("Language"));
  model->setHeaderData(HD_Status, Qt::Horizontal, tr("Status"));
  model->setHeaderData(HD_Text, Qt::Horizontal, tr("Text"));
  model->setHeaderData(HD_UpdateTime, Qt::Horizontal, tr("UpdateTime"));
  model->setHeaderData(HD_Flags, Qt::Horizontal, tr("Flags"));
  model->setHeaderData(HD_BlessedCount, Qt::Horizontal, tr("BlessedCount"));
  model->setHeaderData(HD_CursedCount, Qt::Horizontal, tr("CursedCount"));
  model->setHeaderData(HD_BlockedCount, Qt::Horizontal, tr("BlockedCount"));
}

// - Properties -

QModelIndex
TokenView::currentIndex() const
{ return proxyView_->currentIndex(); }

const Token&
TokenView::token() const
{ return token_; }

void
TokenView::setToken(const Token &token)
{
  token_ = token;
  invalidateTokenLabels();
}

void
TokenView::clearToken()
{
  token_.setId(0);
  invalidateTokenLabels();
}

bool
TokenView::hasToken() const
{ return token_.hasId(); }

void
TokenView::setAliases(const AliasList &l)
{
  clearAliases();
  addAliases(l);
}

void
TokenView::addAlias(const Alias &a)
{
#define FORMAT_TIME(_secs)        QDateTime::fromMSecsSinceEpoch(_secs * 1000)
#define FORMAT_TYPE(_type)        aliasTypeToString(_type)
#define FORMAT_LANGUAGE(_lang)    languageToString(_lang)
#define FORMAT_FLAGS(_flags)      aliasFlagsToStringList(_flags)
#define FORMAT_STATUS(_status)    aliasStatusToString(_status)

  sourceModel_->insertRow(0);
  sourceModel_->setData(sourceModel_->index(0, HD_Type), FORMAT_TYPE(a.aliasType()));
  sourceModel_->setData(sourceModel_->index(0, HD_Language), FORMAT_LANGUAGE(a.language()));
  sourceModel_->setData(sourceModel_->index(0, HD_Status), FORMAT_STATUS(a.status()));
  sourceModel_->setData(sourceModel_->index(0, HD_Text), a.text());
  sourceModel_->setData(sourceModel_->index(0, HD_UpdateTime), FORMAT_TIME(a.updateTime()));
  sourceModel_->setData(sourceModel_->index(0, HD_Flags), FORMAT_FLAGS(a.flags()));
  sourceModel_->setData(sourceModel_->index(0, HD_BlessedCount), a.blessedCount());
  sourceModel_->setData(sourceModel_->index(0, HD_CursedCount), a.cursedCount());
  sourceModel_->setData(sourceModel_->index(0, HD_BlockedCount), a.blockedCount());

  for (int i = 0; i < HD_Count; i++)
    sourceModel_->setData(sourceModel_->index(0, i), sourceModel_->headerData(i, Qt::Horizontal), Qt::ToolTipRole);

#undef FORMAT_TIME
#undef FORMAT_POS
#undef FORMAT_TYPE
#undef FORMAT_LANGUAGE
#undef FORMAT_FLAGS
#undef FORMAT_STATUS
}

void
TokenView::submitAlias(const QString &alias, int type, quint32 language)
{
  Alias a;
  a.setTokenId(token_.id());
  a.setAliasType(type);
  a.setLanguage(language);
  a.setText(alias);
  a.setUserId(userId_);
  a.setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);

  addAlias(a);

  emit aliasSubmitted(a);
}

void
TokenView::addAliases(const AliasList &l)
{
  if (!l.empty())
    foreach (const Alias &a, l)
      addAlias(a);
}

void
TokenView::clearAliases()
{
  //if (sourceModel_->rowCount() == 0)
  //  return;
  //sourceModel_->clear();
  //setAliasHeaderData(sourceModel_);

  while (sourceModel_->rowCount())
    sourceModel_->removeRow(0);
}

// - Slots -

void
TokenView::invalidateFilterRegExp()
{
  QRegExp::PatternSyntax syntax =
      QRegExp::PatternSyntax(
        filterSyntaxComboBox_->itemData(
          filterSyntaxComboBox_->currentIndex()).toInt());

  QRegExp regExp(filterPatternLineEdit_->text(), Qt::CaseInsensitive, syntax);
  proxyModel_->setFilterRegExp(regExp);
}

void
TokenView::invalidateFilterColumn()
{
  proxyModel_->setFilterKeyColumn(filterColumnComboBox_->currentIndex());
}

// - Events -

void TokenView::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
void TokenView::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
void TokenView::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
void TokenView::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

void
TokenView::invalidateTokenLabels()
{
#define FORMAT_TIME(_secs)      QDateTime::fromMSecsSinceEpoch(_secs * 1000).toString()
#define FORMAT_COUNT(_count)    QString::number(_count)
  if (!hasToken())
    return;

  if (token_.hasCreateTime())
    createDateLabel_->setText(FORMAT_TIME(token_.createTime()));
  else
    createDateLabel_->setText(TR(T_UNKNOWN));

  visitedCountLabel_->setText(FORMAT_COUNT(token_.visitedCount()));
  blessedCountLabel_->setText(FORMAT_COUNT(token_.blessedCount()));
  cursedCountLabel_->setText(FORMAT_COUNT(token_.cursedCount()));

#undef FORMAT_TIME
#undef FORMAT_COUNT
}

void
TokenView::bless()
{
  // TODO
}

void
TokenView::curse()
{
  // TODO
}

void
TokenView::addAlias()
{ aliasDialog_->show(); }

// - Format -

// - Formatter -

QString
TokenView::languageToString(int lang)
{
  switch(lang) {
  case Traits::AnyLanguage:     return tr("Any");
  case Traits::English:         return tr("English");
  case Traits::Japanese:        return tr("Japanese");
  case Traits::Chinese:         return tr("Chinese");
  case Traits::Korean:          return tr("Korean");

  case Traits::UnknownLanguage:
  default : return tr("Alien");
  }
}

QStringList
TokenView::aliasFlagsToStringList(int flags)
{
  Q_UNUSED(flags);
  return QStringList(tr("n/a"));
}

QString
TokenView::aliasStatusToString(int status)
{
  switch (status) {
  case Alias::AS_Deleted:  return tr("deleted");
  case Alias::AS_Active:   return tr("active");
  case Alias::AS_Blocked:  return tr("blocked");
  default: return tr("n/a");
  }
}

QString
TokenView::aliasTypeToString(int type)
{
  switch (type) {
  case Alias::AT_Name:  return tr("name");
  case Alias::AT_Source:return tr("source");
  case Alias::AT_Tag:   return tr("tag");
  default: return tr("n/a");
  }
}

// EOF
