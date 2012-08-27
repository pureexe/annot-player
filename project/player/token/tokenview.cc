// tokenview.cc
// 8/13/2011

#include "tokenview.h"
#include "addaliasdialog.h"
#include "datamanager.h"
#include "tr.h"
#include "rc.h"
#include "global.h"
#include "project/common/acui.h"
#include "project/common/acfilteredtableview.h"
#include "module/annotservice/annotserveragent.h"
#include "module/qtext/layoutwidget.h"
#include <QtGui>

using namespace AnnotCloud;

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint)

TokenView::TokenView(DataManager *data, AnnotationServerAgent *server, SignalHub *hub, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), active_(false), data_(data), server_(server), hub_(hub),
    contextMenu_(nullptr), aliasDialog_(nullptr)
{
  Q_ASSERT(data_);
  Q_ASSERT(server_);
  setWindowTitle(tr("Annotation Source"));
  //setAcceptDrops(true);

  // Create models

  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setAliasHeaderData(sourceModel_);

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(sourceModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  }

  tableView_ = new AcFilteredTableView(sourceModel_, proxyModel_, this);

  createLayout();

  // - Connections -
  connect(this, SIGNAL(aliasDeletedWithId(qint64)), data_, SLOT(removeAliasWithId(qint64)));

  //connect(this, SIGNAL(aliasSubmitted(Alias)), data_, SLOT(addAlias(Alias)));

  // Set initial states

  tableView_->sortByColumn(HD_Type, Qt::AscendingOrder);
  tableView_->setCurrentColumn(HD_Text);
}

void
TokenView::setActive(bool t)
{
  if (active_ != t) {
    active_ = t;
    if (active_) {
      connect(data_, SIGNAL(tokenChanged(Token)), SLOT(invalidateToken()), Qt::QueuedConnection);
      connect(data_, SIGNAL(aliasesChanged(AliasList)), SLOT(setAliases(AliasList)), Qt::QueuedConnection);
      connect(data_, SIGNAL(aliasAdded(Alias)), SLOT(addAlias(Alias)), Qt::QueuedConnection);
    } else {
      disconnect(data_, SIGNAL(tokenChanged(Token)), this, SLOT(invalidateToken()));
      disconnect(data_, SIGNAL(aliasesChanged(AliasList)), this, SLOT(setAliases(AliasList)));
      disconnect(data_, SIGNAL(aliasAdded(Alias)), this, SLOT(addAlias(Alias)));
    }
  }
}

void
TokenView::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

//#define MAKE_TOKEN_LABEL(_id, _styleid)
//  _id##Label_ = ui->makeLabel(AcUi::HighlightHint, TR(T_UNKNOWN), TR(T_TOOLTIP_##_styleid));
//  QLabel *_id##Buddy = ui->makeLabel(AcUi::BuddyHint, TR(T_LABEL_##_styleid), TR(T_TOOLTIP_##_styleid), _id##Label_);
//
//  //MAKE_TOKEN_LABEL(source, SOURCE)
//  MAKE_TOKEN_LABEL(createDate, CREATEDATE)
//  MAKE_TOKEN_LABEL(blessCount, BLESSCOUNT)
//  MAKE_TOKEN_LABEL(curseCount, CURSECOUNT)
//  MAKE_TOKEN_LABEL(visitCount, VISITCOUNT)
//  MAKE_TOKEN_LABEL(annotCount, ANNOTCOUNT)
//#undef MAKE_TOKEN_LABEL

  //QToolButton *blessButton = ui->makeToolButton(
  //      AcUi::PushHint, TR(T_BLESS), TR(T_TOOLTIP_BLESSTHISTOKEN), this, SLOT(bless()));
  //QToolButton *curseButton = ui->makeToolButton(
  //      AcUi::PushHint, TR(T_CURSE), TR(T_TOOLTIP_CURSETHISTOKEN), this, SLOT(curse()));
  addButton_ = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, TR(T_ADD), tr("Add Source"), this, SLOT(addAlias()));
  updateButton_ = ui->makeToolButton(
        AcUi::PushHint, TR(T_UPDATE), tr("Update Annotations"), this, SIGNAL(updateAnnotationsRequested()));

  //sourceButton_ = ui->makeToolButton(AcUi::UrlHint, "", TR(T_SOURCE), this, SLOT(openSource()));

  //QLabel *sourceBuddy = ui->makeLabel(AcUi::BuddyHint, TR(T_LABEL_SOURCE), TR(T_TOOLTIP_SOURCE), sourceButton_),
  //       *aliasBuddy = ui->makeLabel(AcUi::BuddyHint, TR(T_LABEL_ALIAS), TR(T_TOOLTIP_ALIAS), addAliasButton);

  // Set layout

  QGridLayout *grid = new QGridLayout; {
    // (row, col, rowspan, colspan, alignment)
    int r, c;
    /*
    grid->addWidget(sourceBuddy, r=0, c=0);
    grid->addWidget(sourceButton_, r, ++c);

    grid->addWidget(createDateBuddy, ++r, c=0);
    grid->addWidget(createDateLabel_, r, ++c);

    grid->addWidget(visitCountBuddy, ++r, c=0);
    grid->addWidget(visitCountLabel_, r, ++c);

    grid->addWidget(annotCountBuddy, ++r, c=0);
    grid->addWidget(annotCountLabel_, r, ++c);

    grid->addWidget(blessCountBuddy, ++r, c=0);
    grid->addWidget(blessCountLabel_, r, ++c);
    grid->addWidget(blessButton, r, ++c);

    grid->addWidget(curseCountBuddy, ++r, c=0);
    grid->addWidget(curseCountLabel_, r, ++c);
    grid->addWidget(curseButton, r, ++c);

    grid->addWidget(aliasBuddy, ++r, c=0);
    ++c;
    grid->addWidget(addAliasButton, r, ++c);
    */

    grid->addWidget(updateButton_, r=0, c=0);
    ++c;
    grid->addWidget(addButton_, r, ++c);

    grid->addWidget(tableView_, ++r, c=0, 1, 3);

    grid->setContentsMargins(6, 6, 6, 6);
    //setContentsMargins(0, 0, 0, 0);
  } setCentralWidget(new LayoutWidget(grid, this));
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
  model->setHeaderData(HD_BlessCount, Qt::Horizontal, TR(T_BLESSCOUNT));
  model->setHeaderData(HD_CurseCount, Qt::Horizontal, TR(T_CURSECOUNT));
  model->setHeaderData(HD_BlockCount, Qt::Horizontal, TR(T_BLOCKCOUNT));
}

void
TokenView::createContextMenu()
{
  // Create menus
  contextMenu_ = new QMenu(this);
  AcUi::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true

  copyAliasAct_ = contextMenu_->addAction(TR(T_COPY), this, SLOT(copyAlias()));
  openAliasUrlAct_ = contextMenu_->addAction(TR(T_OPEN), this, SLOT(openAliasUrl()));

  searchMenu_ = contextMenu_->addMenu(tr("Search") + " ...");
  searchMenu_->setToolTip(tr("Search"));

  contextMenu_->addSeparator();
  deleteAliasAct_ = contextMenu_->addAction(TR(T_DELETE), this, SLOT(deleteAlias()));

  createSearchMenu();
}

void
TokenView::createSearchEngines()
{
  enum { LastEngine = SearchEngineFactory::WikiZh };
  for (int i = 0; i <= LastEngine; i++) {
    SearchEngine *e = SearchEngineFactory::globalInstance()->create(i);
    e->setParent(this);
    searchEngines_.append(e);
  }
}

void
TokenView::createSearchMenu()
{
  if (searchEngines_.isEmpty())
    createSearchEngines();

  SearchEngine *e = searchEngines_[SearchEngineFactory::Google];
  searchMenu_->addAction(QIcon(e->icon()), e->name(), this, SLOT(searchAliasWithGoogle()));

  e = searchEngines_[SearchEngineFactory::GoogleImages];
  searchMenu_->addAction(QIcon(e->icon()), e->name(), this, SLOT(searchAliasWithGoogleImages()));

  e = searchEngines_[SearchEngineFactory::Bing];
  searchMenu_->addAction(QIcon(e->icon()), e->name(), this, SLOT(searchAliasWithBing()));

  searchMenu_->addSeparator();

  e = searchEngines_[SearchEngineFactory::Nicovideo];
  searchMenu_->addAction(QIcon(e->icon()), e->name(), this, SLOT(searchAliasWithNicovideo()));

  e = searchEngines_[SearchEngineFactory::Bilibili];
  searchMenu_->addAction(QIcon(e->icon()), e->name(), this, SLOT(searchAliasWithBilibili()));

  e = searchEngines_[SearchEngineFactory::Acfun];
  searchMenu_->addAction(QIcon(e->icon()), e->name(), this, SLOT(searchAliasWithAcfun()));

  e = searchEngines_[SearchEngineFactory::Youtube];
  searchMenu_->addAction(QIcon(e->icon()), e->name(), this, SLOT(searchAliasWithYoutube()));

  e = searchEngines_[SearchEngineFactory::Youku];
  searchMenu_->addAction(QIcon(e->icon()), e->name(), this, SLOT(searchAliasWithYouku()));

  searchMenu_->addSeparator();

  e = searchEngines_[SearchEngineFactory::WikiEn];
  searchMenu_->addAction(QIcon(e->icon()), e->name(), this, SLOT(searchAliasWithWikiEn()));

  e = searchEngines_[SearchEngineFactory::WikiJa];
  searchMenu_->addAction(QIcon(e->icon()), e->name(), this, SLOT(searchAliasWithWikiJa()));

  e = searchEngines_[SearchEngineFactory::WikiZh];
  searchMenu_->addAction(QIcon(e->icon()), e->name(), this, SLOT(searchAliasWithWikiZh()));
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

int
TokenView::currentAliasType() const
{
  QModelIndex index = currentIndex();
  if (!index.isValid())
    return 0;

  int row = index.row();
  index = index.sibling(row, HD_Type);
  if (!index.isValid())
    return 0;

  QString t = index.data().toString();
  return aliasTypeFromString(t);
}

qint64
TokenView::currentAliasId() const
{
  QModelIndex index = currentIndex();
  if (!index.isValid())
    return 0;

  int row = index.row();
  index = index.sibling(row, HD_Id);
  return index.isValid() ? index.data().toLongLong() : 0;
}

qint64
TokenView::currentAliasUserId() const
{
  QModelIndex index = currentIndex();
  if (!index.isValid())
    return 0;

  int row = index.row();
  index = index.sibling(row, HD_UserId);
  return index.isValid() ? index.data().toLongLong() : 0;
}

void
TokenView::invalidateToken()
{
  updateLabels();
  updateButtons();
  if (aliasDialog_ && aliasDialog_->isVisible())
    aliasDialog_->refresh();
}

//bool
//TokenView::hasToken() const
//{ return token_.hasId(); }

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
  sourceModel_->setData(sourceModel_->index(0, HD_BlessCount), a.blessCount());
  sourceModel_->setData(sourceModel_->index(0, HD_CurseCount), a.curseCount());
  sourceModel_->setData(sourceModel_->index(0, HD_BlockCount), a.blockCount());

  for (int i = 0; i < HD_Count; i++)
    sourceModel_->setData(sourceModel_->index(0, i), sourceModel_->headerData(i, Qt::Horizontal), Qt::ToolTipRole);

#undef FORMAT_TIME
#undef FORMAT_TYPE
#undef FORMAT_LANGUAGE
#undef FORMAT_FLAGS
#undef FORMAT_STATUS

  if (isVisible())
    tableView_->updateCount();
}

void
TokenView::submitAlias(const QString &alias, int type, qint32 language)
{
  const Token &t = data_->token();
  Alias a;
  a.setTokenId(t.id());
  a.setTokenDigest(t.digest());
  a.setTokenSection(t.section());
  a.setType(type);
  a.setLanguage(language);
  a.setText(alias);
  a.setUserId(server_->user().id());
  a.setUpdateTime(QDateTime::currentMSecsSinceEpoch() / 1000);

  //addAlias(a);

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

//void TokenView::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
//void TokenView::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
//void TokenView::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
//void TokenView::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

void
TokenView::updateButtons()
{
  const Token &t = data_->token();
  bool modifiable = t.hashId() || t.hasDigest();
  addButton_->setEnabled(modifiable && server_->isConnected());
  updateButton_->setEnabled(modifiable);
}

void
TokenView::updateLabels()
{
#define FORMAT_TIME(_secs)      QDateTime::fromMSecsSinceEpoch(_secs * 1000).toString()
#define FORMAT_COUNT(_count)    QString::number(_count)
  //if (!hasToken())
  //  return;

  //sourceButton_->setText(token_.source());

  //if (token_.hasCreateTime())
  //  createDateLabel_->setText(FORMAT_TIME(token_.createTime()));
  //else
  //  createDateLabel_->setText(TR(T_UNKNOWN));
//
  //visitCountLabel_->setText(FORMAT_COUNT(token_.visitCount()));
  //annotCountLabel_->setText(FORMAT_COUNT(token_.annotCount()));
  //blessCountLabel_->setText(FORMAT_COUNT(token_.blessCount()));
  //curseCountLabel_->setText(FORMAT_COUNT(token_.curseCount()));

#undef FORMAT_TIME
#undef FORMAT_COUNT
}

//void
//TokenView::setSource(const QString &source)
//{
//  token_.setSource(source);
//  //sourceButton_->setText(source);
//}

/*
void
TokenView::bless()
{
  if (!hasToken())
    return;

  if (!server_->isConnected() || !server_->isAuthorized()) {
    emit warning(tr("cannot perform cast when offline"));
    return;
  }

  qint64 tid = token_.id();
  if (server_->isTokenBlessedWithId(tid)) {
    emit message(tr("token is already blessed"));
    return;
  }

  bool ok;
  qint64 count = blessCountLabel_->text().toUInt(&ok);
  if (!ok)
    count = 0;
  blessCountLabel_->setText(QString::number(++count));

  emit tokenBlessedWithId(tid);
}

void
TokenView::curse()
{
  if (!hasToken())
    return;

  if (!server_->isConnected() || !server_->isAuthorized()) {
    emit warning(tr("cannot perform cast when offline"));
    return;
  }

  qint64 tid = token_.id();
  if (server_->isTokenCursedWithId(tid)) {
    emit message(tr("token is already cursed"));
    return;
  }

  bool ok;
  qint64 count = curseCountLabel_->text().toUInt(&ok);
  if (!ok)
    count = 0;
  curseCountLabel_->setText(QString::number(++count));

  emit tokenCursedWithId(tid);
}
*/

void
TokenView::addAlias()
{
  if (!aliasDialog_) {
    aliasDialog_ = new AddAliasDialog(hub_, this);
    connect(aliasDialog_, SIGNAL(aliasAdded(QString,int,qint32)), SLOT(submitAlias(QString,int,qint32)));
  }
  aliasDialog_->show();
}

// - Formatter -

QString
TokenView::languageToString(int lang)
{
  switch(lang) {
  //case Traits::AnyLanguage:     return TR(T_ANYLANGUAGE);
  case Traits::English:         return TR(T_ENGLISH);
  case Traits::Japanese:        return TR(T_JAPANESE);
  case Traits::Chinese:         return TR(T_CHINESE);
  case Traits::Korean:          return TR(T_KOREAN);
  case Traits::French:          return TR(T_FRENCH);
  case Traits::German:          return TR(T_GERMAN);
  case Traits::Italian:         return TR(T_ITALIAN);
  case Traits::Spanish:         return TR(T_SPANISH);
  case Traits::Portuguese:      return TR(T_PORTUGUESE);
  case Traits::Russian:         return TR(T_RUSSIAN);

  //case Traits::UnknownLanguage:
  default: return TR(T_ALIEN);
  }
}

QStringList
TokenView::aliasFlagsToStringList(int flags)
{
  Q_UNUSED(flags)
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
  case Alias::AT_Title:  return tr("Title");
  case Alias::AT_File:return tr("File");
  case Alias::AT_Tag:   return tr("Tag");
  case Alias::AT_Url: return tr("URL");
  case Alias::AT_Name:  return tr("Name");
  case Alias::AT_Folder: return tr("Folder");
  case Alias::AT_Brand: return tr("Brand");
  default: return TR(T_NA);
  }
}

int
TokenView::aliasTypeFromString(const QString &text)
{
  for (int i = 1; i < Alias::AT_Count; i++)
    if (text == aliasTypeToString(i))
      return i;
  return 0;
}

// - Context menu -

void
TokenView::deleteAlias()
{
  qint64 id = currentAliasId();
  if (id) {
    qint64 userId = server_->user().id();
    if (!userId || userId != currentAliasUserId()) {
      warn(tr("cannot delete other's alias"));
      return;
    }
    tableView_->removeCurrentRow();
    showMessage(tr("don't forget to update annotations!"));
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
    showMessage(TR(T_SUCCEED_COPIED) + ": " + text);
  } else
    warn(TR(T_ERROR_CLIPBOARD_UNAVAILABLE));
}

void
TokenView::contextMenuEvent(QContextMenuEvent *event)
{
  if (!currentIndex().isValid())
    return;

  if (!contextMenu_)
    createContextMenu();

  qint64 userId = server_->user().id();
  deleteAliasAct_->setVisible(userId && userId == currentAliasUserId());

  QString t = currentAliasText();
  copyAliasAct_->setText(TR(T_COPY) + ": " + t);

  bool isUrl = currentAliasType() == Alias::AT_Url;
  openAliasUrlAct_->setVisible(isUrl);
  if (isUrl)
    openAliasUrlAct_->setText(TR(T_OPEN) + ": " + t);

  searchMenu_->setEnabled(!isUrl);

  contextMenu_->popup(event->globalPos());
  event->accept();
}

void
TokenView::openAliasUrl()
{
  QString url = currentAliasText();
  if (!url.isEmpty()) {
    showMessage(TR(T_MENUTEXT_OPENURL) + ": " + url);
    emit openUrlRequested(url);
  }
}

void
TokenView::refresh()
{
  setAliases(data_->aliases());
  //setToken(data_->token());
  //if (!isVisible()) { // already refreshed when visible
  //  updateLabels();
  //  updateButtons();
  //}
  tableView_->updateCount();
  invalidateToken();
}

void
TokenView::setVisible(bool visible)
{
  if (visible)
    refresh();
  setActive(visible);
  Base::setVisible(visible);
}

// EOF
