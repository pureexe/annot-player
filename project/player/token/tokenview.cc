// tokenview.cc
// 8/13/2011

#include "tokenview.h"
#include "addaliasdialog.h"
#include "tr.h"
#include "rc.h"
#include "global.h"
#include "logger.h"
#include "ac/acui.h"
#include "ac/acfilteredtableview.h"
#include "module/serveragent/serveragent.h"
#include <QtGui>

using namespace AnnotCloud;
using namespace Logger;

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint)

TokenView::TokenView(ServerAgent *server, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), server_(server)
{
  Q_ASSERT(server_);
  setWindowTitle(TR(T_TITLE_TOKENVIEW));
  setAcceptDrops(true);

  // Create models

  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setAliasHeaderData(sourceModel_);

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(sourceModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  }

  tableView_ = new AcFilteredTableView(sourceModel_, proxyModel_, this);

  // Create widgets
  aliasDialog_ = new AddAliasDialog(this);
  connect(aliasDialog_, SIGNAL(aliasAdded(QString,int,qint32)), SLOT(submitAlias(QString,int,qint32)));

  createLayout();

  // Create context menu
  createActions();

  // Shortcuts
  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));

  // Set initial states

  tableView_->sortByColumn(HD_Type, Qt::AscendingOrder);
  tableView_->setCurrentColumn(HD_Text);
}

void
TokenView::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);

#define MAKE_TOKEN_LABEL(_id, _styleid) \
  _id##Label_ = ui->makeLabel(AcUi::HighlightHint, TR(T_UNKNOWN), TR(T_TOOLTIP_##_styleid)); \
  QLabel *_id##Buddy = ui->makeLabel(AcUi::BuddyHint, TR(T_LABEL_##_styleid), TR(T_TOOLTIP_##_styleid), _id##Label_);

  //MAKE_TOKEN_LABEL(source, SOURCE)
  MAKE_TOKEN_LABEL(createDate, CREATEDATE)
  MAKE_TOKEN_LABEL(blessedCount, BLESSEDCOUNT)
  MAKE_TOKEN_LABEL(cursedCount, CURSEDCOUNT)
  MAKE_TOKEN_LABEL(visitedCount, VISITEDCOUNT)
  MAKE_TOKEN_LABEL(annotCount, ANNOTCOUNT)
#undef MAKE_TOKEN_LABEL

  QToolButton *blessButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_BLESS), TR(T_TOOLTIP_BLESSTHISTOKEN), this, SLOT(bless()));
  QToolButton *curseButton = ui->makeToolButton(
        AcUi::PushHint, TR(T_CURSE), TR(T_TOOLTIP_CURSETHISTOKEN), this, SLOT(curse()));
  QToolButton *addAliasButton = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_ADD), TR(T_TOOLTIP_ADDALIAS), this, SLOT(addAlias()));

  sourceButton_ = ui->makeToolButton(AcUi::UrlHint, "", TR(T_SOURCE), this, SLOT(openSource()));

  QLabel *sourceBuddy = ui->makeLabel(AcUi::BuddyHint, TR(T_LABEL_SOURCE), TR(T_TOOLTIP_SOURCE), sourceButton_),
         *aliasBuddy = ui->makeLabel(AcUi::BuddyHint, TR(T_LABEL_ALIAS), TR(T_TOOLTIP_ALIAS), addAliasButton);

  // Set layout

  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    grid->addWidget(sourceBuddy, r=0, c=0);
    grid->addWidget(sourceButton_, r, ++c);

    grid->addWidget(createDateBuddy, ++r, c=0);
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

    grid->setContentsMargins(6, 6, 6, 6);
    //setContentsMargins(0, 0, 0, 0);
  } setLayout(grid);
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
  model->setHeaderData(HD_Id, Qt::Horizontal, TR(T_ID));
  model->setHeaderData(HD_UserId, Qt::Horizontal, TR(T_USER_ID));
  model->setHeaderData(HD_BlessedCount, Qt::Horizontal, TR(T_BLESSEDCOUNT));
  model->setHeaderData(HD_CursedCount, Qt::Horizontal, TR(T_CURSEDCOUNT));
  model->setHeaderData(HD_BlockedCount, Qt::Horizontal, TR(T_BLOCKEDCOUNT));
}

void
TokenView::createActions()
{
#define MAKE_ACTION(_action, _styleid, _slot) \
  _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  connect(_action, SIGNAL(triggered()), _slot);

  MAKE_ACTION(copyAliasAct_,  COPY,  SLOT(copyAlias()))
  MAKE_ACTION(deleteAliasAct_,DELETE,SLOT(deleteAlias()))

#undef MAKE_ACTION

  // Create menus
  contextMenu_ = new QMenu(TR(T_TITLE_TOKENVIEW), this);
  AcUi::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true
}

// - Properties -

QModelIndex
TokenView::currentIndex() const
{ return tableView_->currentIndex(); }

QString
TokenView::currentAliasText() const
{
  QModelIndex index = currentIndex();
  if (!index.isValid())
    return QString();

  int row = index.row();
  index = index.sibling(row, HD_Text);
  if (!index.isValid())
    return QString();

  return index.data().toString();
}

qint64
TokenView::currentAliasId() const
{
  QModelIndex index = currentIndex();
  if (!index.isValid())
    return 0;

  int row = index.row();
  index = index.sibling(row, HD_Id);
  if (!index.isValid())
    return 0;
  bool ok;
  qint64 ret = index.data().toLongLong(&ok);
  if (!ok)
    return 0;

  return ret;
}

qint64
TokenView::currentAliasUserId() const
{
  QModelIndex index = currentIndex();
  if (!index.isValid())
    return 0;

  int row = index.row();
  index = index.sibling(row, HD_UserId);
  if (!index.isValid())
    return 0;
  bool ok;
  qint64 ret = index.data().toLongLong(&ok);
  if (!ok)
    return 0;

  return ret;
}

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
  sourceModel_->setData(sourceModel_->index(0, HD_Id), a.id());
  sourceModel_->setData(sourceModel_->index(0, HD_UserId), a.userId());
  sourceModel_->setData(sourceModel_->index(0, HD_BlessedCount), a.blessedCount());
  sourceModel_->setData(sourceModel_->index(0, HD_CursedCount), a.cursedCount());
  sourceModel_->setData(sourceModel_->index(0, HD_BlockedCount), a.blockedCount());

  for (int i = 0; i < HD_Count; i++)
    sourceModel_->setData(sourceModel_->index(0, i), sourceModel_->headerData(i, Qt::Horizontal), Qt::ToolTipRole);

#undef FORMAT_TIME
#undef FORMAT_TYPE
#undef FORMAT_LANGUAGE
#undef FORMAT_FLAGS
#undef FORMAT_STATUS

  if (isVisible())
    tableView_->invalidateCount();
}

void
TokenView::submitAlias(const QString &alias, int type, qint32 language)
{
  Alias a;
  a.setTokenId(token_.id());
  a.setTokenDigest(token_.digest());
  a.setTokenPart(token_.part());
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

  sourceButton_->setText(token_.source());

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
TokenView::setSource(const QString &source)
{
  token_.setSource(source);
  sourceButton_->setText(source);
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
  case Alias::AT_Url: return tr("URL");
  default: return TR(T_NA);
  }
}

// - Context menu -

void
TokenView::deleteAlias()
{
  qint64 id = currentAliasId();
  if (id) {
    qint64 userId = server_->user().id();
    if (!userId || userId != currentAliasUserId()) {
      tr("cannot delete other's alias");
      return;
    }
    tableView_->removeCurrentRow();
    emit aliasDeletedWithId(id);
  }
}

void
TokenView::copyAlias()
{
  QString text = currentAliasText();
  if (text.isEmpty())
    return;

  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard) {
    clipboard->setText(text);
    log(TR(T_SUCCEED_COPIED) + ": " + text);
  } else
    warn(TR(T_ERROR_CLIPBOARD_UNAVAILABLE));
}

void
TokenView::contextMenuEvent(QContextMenuEvent *event)
{
  if (!event)
    return;
  if (!currentIndex().isValid())
    return;

  contextMenu_->clear();

  contextMenu_->addAction(copyAliasAct_);

  qint64 userId = server_->user().id();
  if (userId && userId == currentAliasUserId())
    contextMenu_->addAction(deleteAliasAct_);

  // Pop up
  contextMenu_->popup(event->globalPos());
  event->accept();
}

void
TokenView::openSource()
{
  sourceButton_->setChecked(true);
  QString url = sourceButton_->text();
  if (!url.isEmpty()) {
    log(TR(T_MENUTEXT_OPENURL) + ": " + url);
    QDesktopServices::openUrl(url);
  }
}

void
TokenView::setVisible(bool visible)
{
  if (visible)
    tableView_->invalidateCount();
  Base::setVisible(visible);
}

// EOF
