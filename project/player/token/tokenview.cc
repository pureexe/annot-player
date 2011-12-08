// tokenview.cc
// 8/13/2011

#include "tokenview.h"
#include "addaliasdialog.h"
#include "tr.h"
#include "stylesheet.h"
#include "uistyle.h"
#include "global.h"
#include "logger.h"
#include "filteredtableview.h"
#include "module/serveragent/serveragent.h"
#include "core/gui/toolbutton.h"
#include <QtGui>

using namespace Core::Cloud;
using namespace Logger;

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

TokenView::TokenView(ServerAgent *server, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), server_(server)
{
  Q_ASSERT(server_);
  setWindowTitle(TR(T_TITLE_TOKENVIEW));
  UiStyle::globalInstance()->setWindowStyle(this);
  setAcceptDrops(true);

  // Create models

  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setAliasHeaderData(sourceModel_);
  tableView_ = new FilteredTableView(sourceModel_, this);

  // Create widgets
  aliasDialog_ = new AddAliasDialog(this);
  connect(aliasDialog_, SIGNAL(aliasAdded(QString,int,quint32)), SLOT(submitAlias(QString,int,quint32)));

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
  MAKE_TOKEN_LABEL(annotCount, ANNOTCOUNT)
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

  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    grid->addWidget(createDateBuddy, r=0, c=0);
    grid->addWidget(createDateLabel_, r, ++c);

    grid->addWidget(visitedCountBuddy, ++r, c=0);
    grid->addWidget(visitedCountLabel_, r, ++c);

    grid->addWidget(annotCountBuddy, ++r, c=0);
    grid->addWidget(annotCountLabel_, r, ++c);

    grid->addWidget(blessedCountBuddy, ++r, c=0);
    grid->addWidget(blessedCountLabel_, r, ++c);
    grid->addWidget(blessButton, r, ++c);

    grid->addWidget(cursedCountBuddy, ++r, c=0);
    grid->addWidget(cursedCountLabel_, r, ++c);
    grid->addWidget(curseButton, r, ++c);

    grid->addWidget(aliasBuddy, ++r, c=0);
    ++c;
    grid->addWidget(addAliasButton, r, ++c);

    grid->addWidget(tableView_, ++r, c=0, 1, 3);

    //grid->setContentsMargins(0, 0, 0, 0);
    //setContentsMargins(0, 0, 0, 0);
  } setLayout(grid);

  // Set initial states

  tableView_->sortByColumn(HD_Type, Qt::AscendingOrder);
  tableView_->setCurrentColumn(HD_Text);
}

void
TokenView::setAliasHeaderData(QAbstractItemModel *model)
{
  Q_ASSERT(model);
  model->setHeaderData(HD_Text, Qt::Horizontal, TR(T_TEXT));
  model->setHeaderData(HD_Type, Qt::Horizontal, TR(T_TYPE));
  model->setHeaderData(HD_UpdateTime, Qt::Horizontal, TR(T_UPDATETIME));
  model->setHeaderData(HD_Language, Qt::Horizontal, TR(T_LANGUAGE));
  model->setHeaderData(HD_Status, Qt::Horizontal, TR(T_STATUS));
  model->setHeaderData(HD_Flags, Qt::Horizontal, TR(T_FLAGS));
  model->setHeaderData(HD_BlessedCount, Qt::Horizontal, TR(T_BLESSEDCOUNT));
  model->setHeaderData(HD_CursedCount, Qt::Horizontal, TR(T_CURSEDCOUNT));
  model->setHeaderData(HD_BlockedCount, Qt::Horizontal, TR(T_BLOCKEDCOUNT));
}

// - Properties -

QModelIndex
TokenView::currentIndex() const
{ return tableView_->currentIndex(); }

const Token&
TokenView::token() const
{ return token_; }

Token&
TokenView::token()
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
  sourceModel_->setData(sourceModel_->index(0, HD_Text), a.text());
  sourceModel_->setData(sourceModel_->index(0, HD_Type), FORMAT_TYPE(a.type()));
  sourceModel_->setData(sourceModel_->index(0, HD_UpdateTime), FORMAT_TIME(a.updateTime()));
  sourceModel_->setData(sourceModel_->index(0, HD_Language), FORMAT_LANGUAGE(a.language()));
  sourceModel_->setData(sourceModel_->index(0, HD_Status), FORMAT_STATUS(a.status()));
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
  a.setTokenDigest(token_.digest());
  a.setTokenDigestType(token_.digestType());
  a.setType(type);
  a.setLanguage(language);
  a.setText(alias);
  a.setUserId(server_->user().id());
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
{ tableView_->clear(); }

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
  annotCountLabel_->setText(FORMAT_COUNT(token_.annotCount()));
  blessedCountLabel_->setText(FORMAT_COUNT(token_.blessedCount()));
  cursedCountLabel_->setText(FORMAT_COUNT(token_.cursedCount()));

#undef FORMAT_TIME
#undef FORMAT_COUNT
}

void
TokenView::bless()
{
  if (!hasToken())
    return;

  if (!server_->isConnected() || !server_->isAuthorized()) {
    warn(tr("cannot perform cast when offline"));
    return;
  }

  qint64 tid = token_.id();
  if (server_->isTokenBlessedWithId(tid)) {
    log(tr("token is already blessed"));
    return;
  }

  bool ok;
  qint64 count = blessedCountLabel_->text().toUInt(&ok);
  if (!ok)
    count = 0;
  blessedCountLabel_->setText(QString::number(++count));

  emit tokenBlessedWithId(tid);
}

void
TokenView::curse()
{
  if (!hasToken())
    return;

  if (!server_->isConnected() || !server_->isAuthorized()) {
    warn(tr("cannot perform cast when offline"));
    return;
  }

  qint64 tid = token_.id();
  if (server_->isTokenCursedWithId(tid)) {
    log(tr("token is already cursed"));
    return;
  }

  bool ok;
  qint64 count = cursedCountLabel_->text().toUInt(&ok);
  if (!ok)
    count = 0;
  cursedCountLabel_->setText(QString::number(++count));

  emit tokenCursedWithId(tid);
}

void
TokenView::addAlias()
{ aliasDialog_->show(); }

// - Formatter -

QString
TokenView::languageToString(int lang)
{
  switch(lang) {
  case Traits::AnyLanguage:     return TR(T_ANYLANGUAGE);
  case Traits::English:         return TR(T_ENGLISH);
  case Traits::Japanese:        return TR(T_JAPANESE);
  case Traits::Chinese:         return TR(T_CHINESE);
  case Traits::Korean:          return TR(T_KOREAN);

  case Traits::UnknownLanguage:
  default : return TR(T_ALIEN);
  }
}

QStringList
TokenView::aliasFlagsToStringList(int flags)
{
  Q_UNUSED(flags);
  return QStringList(TR(T_NA));
}

QString
TokenView::aliasStatusToString(int status)
{
  switch (status) {
  case Alias::AS_Deleted:  return TR(T_DELETED);
  case Alias::AS_Active:   return TR(T_ACTIVE);
  case Alias::AS_Blocked:  return TR(T_BLOCKED);
  default: return TR(T_NA);
  }
}

QString
TokenView::aliasTypeToString(int type)
{
  switch (type) {
  case Alias::AT_Name:  return tr("name");
  case Alias::AT_Source:return tr("source");
  case Alias::AT_Tag:   return tr("tag");
  default: return TR(T_NA);
  }
}

// EOF
