// annotationbrowser.cc
// 10/3/2011

#include "annotationbrowser.h"
#include "annotationeditor.h"
#include "rc.h"
#include "tr.h"
#include "src/common/acfilteredtableview.h"
#include "signalhub.h"
#include "src/common/acui.h"
#include "module/qtext/datetime.h"
#include "module/qtext/layoutwidget.h"
#include "module/annotcloud/traits.h"
#include "module/annotcloud/annottag.h"
#include <QtGui>

using namespace AnnotCloud;

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
  Qt::WindowCloseButtonHint | \
  Qt::WindowStaysOnTopHint)

AnnotationBrowser::AnnotationBrowser(SignalHub *hub, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), hub_(hub), userId_(0), annotationPos_(0), editor_(0), contextMenu_(0)
{
  Q_ASSERT(hub_);

  setWindowTitle(tr("Annotation Browser"));
  //setAcceptDrops(true);

  createModel();
  createLayout();
  //createActions();

  new QShortcut(QKeySequence("CTRL+1"), meButton_, SLOT(click()));
  new QShortcut(QKeySequence("CTRL+2"), nowButton_, SLOT(click()));
  new QShortcut(QKeySequence("CTRL+3"), subtitleButton_, SLOT(click()));
#ifndef Q_OS_MAC
  new QShortcut(QKeySequence("ALT+1"), meButton_, SLOT(click()));
  new QShortcut(QKeySequence("ALT+2"), nowButton_, SLOT(click()));
  new QShortcut(QKeySequence("ALT+3"), subtitleButton_, SLOT(click()));
#endif // Q_OS_MAC

  // Initial states
  tableView_->sortByColumn(HD_CreateTime, Qt::DescendingOrder);
  tableView_->setCurrentColumn(HD_Text);
}

void
AnnotationBrowser::createModel()
{
  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setHeaderData(sourceModel_);

  filterMeModel_ = new QSortFilterProxyModel; {
    filterMeModel_->setSourceModel(sourceModel_);
    filterMeModel_->setDynamicSortFilter(true);
    filterMeModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
    filterMeModel_->setFilterKeyColumn(HD_UserId);
  }
  filterNowModel_ = new QSortFilterProxyModel; {
    filterNowModel_->setSourceModel(filterMeModel_);
    filterNowModel_->setDynamicSortFilter(true);
    filterNowModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
    filterNowModel_->setFilterKeyColumn(HD_Pos);
  }
  filterSubtitleModel_ = new QSortFilterProxyModel; {
    filterSubtitleModel_->setSourceModel(filterNowModel_);
    filterSubtitleModel_->setDynamicSortFilter(true);
    filterSubtitleModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
    filterSubtitleModel_->setFilterKeyColumn(HD_Text);
  }

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(filterSubtitleModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  }

  tableView_ = new AcFilteredTableView(sourceModel_, proxyModel_, this);
}

void
AnnotationBrowser::createLayout()
{
  AcUi *ui = AcUi::globalInstance();

  meButton_ = ui->makeToolButton(
        AcUi::CheckHint, tr("Mine"), tr("Display my annotations only"), K_CTRL "+1", this, SLOT(setMe(bool)));
  nowButton_ = ui->makeToolButton(
        AcUi::CheckHint, tr("Now"), tr("Display annotations at the time only"), K_CTRL "+2", this, SLOT(setNow(bool)));
  subtitleButton_ = ui->makeToolButton(
        AcUi::CheckHint, TR(T_SUBTITLE), tr("Display subtitles only"), K_CTRL "+3", this, SLOT(setSubtitle(bool)));

  QVBoxLayout *rows = new QVBoxLayout; {
    QLayout *header = new QHBoxLayout;
    rows->addLayout(header);
    rows->addWidget(tableView_);

    header->addWidget(meButton_);
    header->addWidget(nowButton_);
    header->addWidget(subtitleButton_);

    rows->setContentsMargins(9, 9, 9, 9);
    setContentsMargins(0, 0, 0, 0);

  } setCentralWidget(new LayoutWidget(rows, this));

  // Set initial states

  tableView_->sortByColumn(HD_CreateTime, Qt::DescendingOrder);
  tableView_->setCurrentColumn(HD_Text);

}

void
AnnotationBrowser::createContextMenu()
{
  // Create menus
  contextMenu_ = new QMenu(this);
  AcUi::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true
  editAnnotAct_ = contextMenu_->addAction(TR(T_MENUTEXT_EDITTHISANNOT), this, SLOT(editAnnotation()));
  copyAnnotAct_ = contextMenu_->addAction(TR(T_MENUTEXT_COPYTHISANNOT), this, SLOT(copyAnnotation()));
  contextMenu_->addSeparator();
  deleteAnnotAct_ = contextMenu_->addAction(TR(T_MENUTEXT_DELETETHISANNOT), this, SLOT(deleteAnnotation()));
  blessAnnotAct_ = contextMenu_->addAction(TR(T_MENUTEXT_BLESSTHISANNOT), this, SLOT(blessAnnotation()));
  curseAnnotAct_ = contextMenu_->addAction(TR(T_MENUTEXT_CURSETHISANNOT), this, SLOT(curseAnnotation()));
  blockAnnotAct_ = contextMenu_->addAction(TR(T_MENUTEXT_BLOCKTHISANNOT), this, SLOT(blockAnnotation()));
  contextMenu_->addSeparator();
  //viewUserAct_ = contextMenu_->addAction(TR(T_MENUTEXT_VIEWTHISUSER), this, SLOT(viewUser()));
  blockUserAct_ = contextMenu_->addAction(TR(T_MENUTEXT_BLOCKTHISUSER), this, SLOT(blockUser()));

  contextMenu_->addSeparator();
  analyzeUserAct_ = contextMenu_->addAction(tr("User Analytics"), this, SLOT(analyzeUser()));
  analyticsAct_ = contextMenu_->addAction(tr("Annotation Analytics"), this, SIGNAL(annotationAnalyticsRequested()));
}


void
AnnotationBrowser::setHeaderData(QAbstractItemModel *model)
{
  Q_ASSERT(model);
  model->setHeaderData(HD_Text, Qt::Horizontal, TR(T_TEXT));
  model->setHeaderData(HD_Pos, Qt::Horizontal, TR(T_POS));
  model->setHeaderData(HD_UserAlias, Qt::Horizontal, TR(T_USER_ALIAS));
  model->setHeaderData(HD_CreateTime, Qt::Horizontal, TR(T_CREATETIME));
  model->setHeaderData(HD_UpdateTime, Qt::Horizontal, TR(T_UPDATETIME));
  model->setHeaderData(HD_Id, Qt::Horizontal, TR(T_ID));
  model->setHeaderData(HD_Language, Qt::Horizontal, TR(T_LANGUAGE));
  model->setHeaderData(HD_Status, Qt::Horizontal, TR(T_STATUS));
  model->setHeaderData(HD_UserId, Qt::Horizontal, TR(T_USER_ID));
  model->setHeaderData(HD_Flags, Qt::Horizontal, TR(T_FLAGS));
  model->setHeaderData(HD_BlessCount, Qt::Horizontal, TR(T_BLESSCOUNT));
  model->setHeaderData(HD_CurseCount, Qt::Horizontal, TR(T_CURSECOUNT));
  model->setHeaderData(HD_BlockCount, Qt::Horizontal, TR(T_BLOCKCOUNT));
}

// - Properties -

AnnotationEditor*
AnnotationBrowser::editor() const
{
  if (!editor_) {
    auto self = const_cast<Self *>(this);
    AnnotationEditor *ret = new AnnotationEditor(self);
    connect(ret, SIGNAL(textSaved(QString)), SLOT(saveAnnotationText(QString)));
    self->editor_ = ret;
  }
  return editor_;
}

QModelIndex
AnnotationBrowser::currentIndex() const
{ return tableView_->currentIndex(); }

qint64
AnnotationBrowser::currentId() const
{
  QModelIndex index = currentIndex();
  if (!index.isValid())
    return 0;

  int row = index.row();
  index = index.sibling(row, HD_Id);
  return index.isValid() ? index.data().toLongLong() : 0;
}

qint64
AnnotationBrowser::currentUserId() const
{
  QModelIndex index = currentIndex();
  if (!index.isValid())
    return 0;

  int row = index.row();
  index = index.sibling(row, HD_UserId);
  return index.isValid() ? index.data().toLongLong() : 0;
}

QString
AnnotationBrowser::currentUserAlias() const
{
  QModelIndex index = currentIndex();
  if (!index.isValid())
    return QString();

  int row = index.row();
  index = index.sibling(row, HD_UserAlias);
  if (!index.isValid())
    return QString();
  return index.data().toString();
}

QString
AnnotationBrowser::currentText() const
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

void
AnnotationBrowser::setAnnotations(const AnnotationList &l)
{
  clear();
  addAnnotations(l);
}

bool
AnnotationBrowser::isEmpty() const
{ return sourceModel_->rowCount() == 0; }

void
AnnotationBrowser::addAnnotation(const Annotation &a)
{
#define FORMAT_TIME(_secs)        QDateTime::fromMSecsSinceEpoch(_secs * 1000).toString(Qt::ISODate)
#define FORMAT_POS(_msecs)        QtExt::msecs2time(_msecs).toString()
#define FORMAT_LANGUAGE(_lang)    languageToString(_lang)
#define FORMAT_FLAGS(_flags)      annotationFlagsToStringList(_flags)
#define FORMAT_STATUS(_status)    annotationStatusToString(_status)

  sourceModel_->insertRow(0);

  sourceModel_->setData(sourceModel_->index(0, HD_Text), a.text(), Qt::DisplayRole);
  if (hub_->isMediaTokenMode())
    sourceModel_->setData(sourceModel_->index(0, HD_Pos), FORMAT_POS(a.pos()), Qt::DisplayRole);
  else
    sourceModel_->setData(sourceModel_->index(0, HD_Pos), a.pos(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_UserAlias), a.userAlias(), Qt::DisplayRole);
  if (a.createTime())
    sourceModel_->setData(sourceModel_->index(0, HD_CreateTime), FORMAT_TIME(a.createTime()), Qt::DisplayRole);
  else
    sourceModel_->setData(sourceModel_->index(0, HD_CreateTime), TR(T_NA), Qt::DisplayRole);
  if (a.updateTime())
    sourceModel_->setData(sourceModel_->index(0, HD_UpdateTime), FORMAT_TIME(a.updateTime()), Qt::DisplayRole);
  else
    sourceModel_->setData(sourceModel_->index(0, HD_UpdateTime), TR(T_NA), Qt::DisplayRole);

  sourceModel_->setData(sourceModel_->index(0, HD_Id), a.id(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Language), FORMAT_LANGUAGE(a.language()), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Status), FORMAT_STATUS(a.status()), Qt::DisplayRole);

  sourceModel_->setData(sourceModel_->index(0, HD_UserId), a.userId(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Flags), FORMAT_FLAGS(a.flags()), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_BlessCount), a.blessCount(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_CurseCount), a.curseCount(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_BlockCount), a.blockCount(), Qt::DisplayRole);

  for (int i = 0; i < HD_Count; i++)
    sourceModel_->setData(sourceModel_->index(0, i), sourceModel_->headerData(i, Qt::Horizontal), Qt::ToolTipRole);

#undef FORMAT_TIME
#undef FORMAT_POS
#undef FORMAT_LANGUAGE
#undef FORMAT_FLAGS
#undef FORMAT_STATUS

  if (isVisible())
    tableView_->updateCount();
}

void
AnnotationBrowser::addAnnotations(const AnnotationList &l)
{
  if (!l.empty())
    foreach (const Annotation &a, l)
      addAnnotation(a);
}

void
AnnotationBrowser::removeAnnotations()
{ tableView_->clear(); }

void
AnnotationBrowser::clear()
{ removeAnnotations(); }

void
AnnotationBrowser::removeAnnotationWithId(qint64 id)
{
  int row = rowWithId(id);
  if (row >= 0) {
    sourceModel_->removeRow(row);
    if (isVisible())
      tableView_->updateCount();
  }
}

int
AnnotationBrowser::rowWithId(qint64 id) const
{
  for (int row = 0; row < sourceModel_->rowCount(); row++) {
    QModelIndex index = sourceModel_->index(row, HD_Id);
    bool ok;
    qint64 rowId = sourceModel_->data(index).toLongLong(&ok);
    if (ok && rowId == id)
      return row;
  }
  return -1;
}

// - Formatter -

QString
AnnotationBrowser::languageToString(int lang)
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
AnnotationBrowser::annotationFlagsToStringList(int flags)
{
  QStringList ret;
  if (flags & Annotation::AF_Anonymous)
    ret.append(TR(T_ANONYMOUS));

  if (ret.isEmpty())
    ret.append(TR(T_NA));
  return ret;
}

QString
AnnotationBrowser::annotationStatusToString(int status)
{
  switch (status) {
  case Annotation::AS_Deleted:  return TR(T_DELETED);
  case Annotation::AS_Active:   return TR(T_ACTIVE);
  case Annotation::AS_Blocked:  return TR(T_BLOCKED);
  default: return TR(T_NA);
  }
}

// - Actions -

void
AnnotationBrowser::takeAnnotationOwnership()
{
  QModelIndex index = currentIndex();
  if (!index.isValid())
    return;

  if (!userId_ || currentUserId() != User::guest().id()) {
    emit warning(tr("cannot modify other's annotation"));
    return;
  }

  int row = index.row();
  QModelIndex mi = index.sibling(row, HD_UserId);
  sourceModel_->setData(mi, userId_, Qt::DisplayRole);

  qint64 id = currentId();
  if (id) {
    updateAnnotationUserIdWithId(userId_, id);
    emit annotationUserIdUpdatedWithId(userId_, id);
  }
}

void
AnnotationBrowser::saveAnnotationText(const QString &text)
{
  if (!editedIndex_.isValid())
    return;

  if (!userId_ || currentUserId() != userId_) {
    emit warning(tr("cannot edit other's annotation"));
    return;
  }

  int row = editedIndex_.row();

  QModelIndex mi = editedIndex_.sibling(row, HD_Text);

  sourceModel_->setData(mi, text, Qt::DisplayRole);
  qint64 id = editor()->id();

  if (id) {
    updateAnnotationTextWithId(text, id);
    emit annotationTextUpdatedWithId(text, id);
  }
}

void
AnnotationBrowser::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  int row = rowWithId(id);
  if (row >= 0) {
    sourceModel_->setData(sourceModel_->index(row, HD_Text), text, Qt::DisplayRole);
    sourceModel_->setData(sourceModel_->index(row, HD_UpdateTime), QDateTime::currentDateTime(), Qt::DisplayRole);
  }
}

void
AnnotationBrowser::updateAnnotationUserIdWithId(qint64 userId, qint64 id)
{
  int row = rowWithId(id);
  if (row >= 0)
    sourceModel_->setData(sourceModel_->index(row, HD_UserId), userId, Qt::DisplayRole);
}

void
AnnotationBrowser::editAnnotation()
{
  // TODO: CHECKPOINT
  qint64 id = currentId();
  if (!id)
    return;

  editedIndex_ = currentIndex();

  AnnotationEditor *e = editor();
  e->setId(id);
  e->setText(currentText());
  e->show();
}

void
AnnotationBrowser::copyAnnotation()
{
  QString text = currentText();
  if (text.isEmpty())
    return;

  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard) {
    clipboard->setText(text);
    emit message(TR(T_SUCCEED_COPIED) + ": " + text);
  } else
    emit warning(TR(T_ERROR_CLIPBOARD_UNAVAILABLE));
}

void
AnnotationBrowser::deleteAnnotation()
{
  if (!userId_ || currentUserId() != userId_) {
    emit warning(tr("cannot delete other's annotation"));
    return;
  }

  qint64 id = currentId();
  tableView_->removeCurrentRow();

  if (id)
    emit annotationDeletedWithId(id);
}

void
AnnotationBrowser::blessAnnotation()
{
  qint64 id = currentId();
  if (id)
    emit annotationBlessedWithId(id);
  qint64 uid = currentUserId();
  if (uid)
    emit userBlessedWithId(uid);
}

void
AnnotationBrowser::curseAnnotation()
{
  qint64 id = currentId();
  if (id)
    emit annotationCursedWithId(id);
  qint64 uid = currentUserId();
  if (uid)
    emit userCursedWithId(uid);
}

void
AnnotationBrowser::blockAnnotation()
{
  QString text = currentText();
  if (!text.isEmpty())
    emit annotationBlockedWithText(text);
  qint64 id = currentId();
  if (id)
    emit annotationBlockedWithId(id);
}

void
AnnotationBrowser::viewUser()
{
  // CHECKPOINT
}

void
AnnotationBrowser::blockUser()
{
  QString text = currentUserAlias();
  if (!text.isEmpty())
    emit userBlockedWithAlias(text);
  qint64 uid = currentUserId();
  if (uid)
    emit userBlockedWithId(uid);
}

void
AnnotationBrowser::analyzeUser()
{
  qint64 uid = currentUserId();
  if (uid)
    emit userAnalyticsRequested(uid);
}

// - Events -

void
AnnotationBrowser::setVisible(bool visible)
{
  if (visible) {
    invalidateFilters();
    tableView_->updateCount();
  }
  Base::setVisible(visible);
}

void
AnnotationBrowser::contextMenuEvent(QContextMenuEvent *event)
{
  if (!currentIndex().isValid())
    return;
  updateContextMenu();
  contextMenu_->popup(event->globalPos());
  event->accept();
}

void
AnnotationBrowser::updateContextMenu()
{
  if (!contextMenu_)
    createContextMenu();
  //editAnnotAct_->setVisible(true);
  copyAnnotAct_->setVisible(!currentText().isEmpty());

  qint64 id = currentId();
  qint64 cuid = currentUserId();

  deleteAnnotAct_->setVisible(id && userId_ == cuid);
  blessAnnotAct_->setVisible(id > 0 && userId_ != cuid);
  curseAnnotAct_->setVisible(id > 0 && userId_ != cuid);
  blockAnnotAct_->setVisible(id && userId_ != cuid);
  //viewUserAct_->setVisible(cuid && cuid != userId_);
  blockUserAct_->setVisible(cuid && cuid != userId_);
  analyzeUserAct_->setVisible(cuid);
  analyticsAct_->setVisible(true);
}

//void AnnotationBrowser::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
//void AnnotationBrowser::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
//void AnnotationBrowser::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
//void AnnotationBrowser::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

// - Filters -

void
AnnotationBrowser::setAnnotationPos(qint64 pos)
{
  annotationPos_ = pos;
  if (isVisible())
    invalidateFilters();
}

void
AnnotationBrowser::setNow(bool t)
{
  QString s;
  if (t) {
    s = hub_->isMediaTokenMode() ?
      QtExt::msecs2time(annotationPos_).toString() :
      QString::number(annotationPos_);
    s = '^' + s + '$';
  }

  filterNowModel_->setFilterRegExp(s);
  tableView_->updateCount();
}

void
AnnotationBrowser::setMe(bool t)
{
  QString s;
  if (t) {
    s = QString::number(userId_);
    s = '^' + s + '$';
  }
  filterMeModel_->setFilterRegExp(s);
  tableView_->updateCount();
}

void
AnnotationBrowser::setSubtitle(bool t)
{
  QString s;
  if (t)
    s = CORE_CMD_SUB;
  filterSubtitleModel_->setFilterFixedString(s);
  tableView_->updateCount();
}

void
AnnotationBrowser::invalidateFilters()
{
  if (nowButton_->isChecked())
    setNow(true);
}

// EOF
/*
void TextEdit::setupFileActions()
{
  QToolBar *tb = new QToolBar(this);
  tb->setWindowTitle(tr("File Actions"));
  addToolBar(tb);

  QMenu *menu = new QMenu(tr("&File"), this);
  menuBar()->addMenu(menu);

  QAction *a;

  QIcon newIcon = QIcon::fromTheme("document-new", QIcon(rsrcPath + "/filenew.png"));
  a = new QAction( newIcon, tr("&New"), this);
  a->setPriority(QAction::LowPriority);
  a->setShortcut(QKeySequence::New);
  connect(a, SIGNAL(triggered()), this, SLOT(fileNew()));
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction(QIcon::fromTheme("document-open", QIcon(rsrcPath + "/fileopen.png")),
          tr("&Open..."), this);
  a->setShortcut(QKeySequence::Open);
  connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
  tb->addAction(a);
  menu->addAction(a);

  menu->addSeparator();

  actionSave = a = new QAction(QIcon::fromTheme("document-save", QIcon(rsrcPath + "/filesave.png")),
                 tr("&Save"), this);
  a->setShortcut(QKeySequence::Save);
  connect(a, SIGNAL(triggered()), this, SLOT(fileSave()));
  a->setEnabled(false);
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction(tr("Save &As..."), this);
  a->setPriority(QAction::LowPriority);
  connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
  menu->addAction(a);
  menu->addSeparator();

#ifndef QT_NO_PRINTER
  a = new QAction(QIcon::fromTheme("document-print", QIcon(rsrcPath + "/fileprint.png")),
          tr("&Print..."), this);
  a->setPriority(QAction::LowPriority);
  a->setShortcut(QKeySequence::Print);
  connect(a, SIGNAL(triggered()), this, SLOT(filePrint()));
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction(QIcon::fromTheme("fileprint", QIcon(rsrcPath + "/fileprint.png")),
          tr("Print Preview..."), this);
  connect(a, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
  menu->addAction(a);

  a = new QAction(QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png")),
  tr("&Export PDF..."), this);
  a->setPriority(QAction::LowPriority);
  a->setShortcut(Qt::CTRL + Qt::Key_D);
  connect(a, SIGNAL(triggered()), this, SLOT(filePrintPdf()));
  tb->addAction(a);
  menu->addAction(a);

  menu->addSeparator();
#endif

  a = new QAction(tr("&Quit"), this);
  a->setShortcut(Qt::CTRL + Qt::Key_Q);
  connect(a, SIGNAL(triggered()), this, SLOT(close()));
  menu->addAction(a);
}
*/

/*
void TextEdit::setupEditActions()
{
  QToolBar *tb = new QToolBar(this);
  tb->setWindowTitle(tr("Edit Actions"));
  addToolBar(tb);
  QMenu *menu = new QMenu(tr("&Edit"), this);
  menuBar()->addMenu(menu);

  QAction *a;
  a = actionUndo = new QAction(QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png")),
                        tr("&Undo"), this);
  a->setShortcut(QKeySequence::Undo);
  tb->addAction(a);
  menu->addAction(a);
  a = actionRedo = new QAction(QIcon::fromTheme("edit-redo", QIcon(rsrcPath + "/editredo.png")),
                        tr("&Redo"), this);
  a->setPriority(QAction::LowPriority);
  a->setShortcut(QKeySequence::Redo);
  tb->addAction(a);
  menu->addAction(a);
  menu->addSeparator();
  a = actionCut = new QAction(QIcon::fromTheme("edit-cut", QIcon(rsrcPath + "/editcut.png")),
                       tr("Cu&t"), this);
  a->setPriority(QAction::LowPriority);
  a->setShortcut(QKeySequence::Cut);
  tb->addAction(a);
  menu->addAction(a);
  a = actionCopy = new QAction(QIcon::fromTheme("edit-copy", QIcon(rsrcPath + "/editcopy.png")),
                 tr("&Copy"), this);
  a->setPriority(QAction::LowPriority);
  a->setShortcut(QKeySequence::Copy);
  tb->addAction(a);
  menu->addAction(a);
  a = actionPaste = new QAction(QIcon::fromTheme("edit-paste", QIcon(rsrcPath + "/editpaste.png")),
                  tr("&Paste"), this);
  a->setPriority(QAction::LowPriority);
  a->setShortcut(QKeySequence::Paste);
  tb->addAction(a);
  menu->addAction(a);
#ifndef QT_NO_CLIPBOARD
  if (const QMimeData *md = QApplication::clipboard()->mimeData())
    actionPaste->setEnabled(md->hasText());
#endif
}

void TextEdit::setCurrentFileName(const QString &fileName)
{
  this->fileName = fileName;
  textEdit_->document()->setModified(false);

  QString shownName;
  if (fileName.isEmpty())
    shownName = "untitled.txt";
  else
    shownName = QFileInfo(fileName).fileName();

  setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Rich Text")));
  setWindowModified(false);
}

void TextEdit::fileNew()
{
  if (maybeSave()) {
    textEdit_->clear();
    setCurrentFileName(QString());
  }
}

void TextEdit::fileOpen()
{
  QString fn = QFileDialog::getOpenFileName(this, tr("Open File..."),
                        QString(), tr("HTML-Files (*.htm *.html);;All Files (*)"));
  if (!fn.isEmpty())
    load(fn);
}

bool TextEdit::fileSave()
{
  if (fileName.isEmpty())
    return fileSaveAs();

  QTextDocumentWriter writer(fileName);
  bool success = writer.write(textEdit_->document());
  if (success)
    textEdit_->document()->setModified(false);
  return success;
}

bool TextEdit::fileSaveAs()
{
  QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
                        QString(), tr("ODF files (*.odt);;HTML-Files (*.htm *.html);;All Files (*)"));
  if (fn.isEmpty())
    return false;
  if (! (fn.endsWith(".odt", Qt::CaseInsensitive) || fn.endsWith(".htm", Qt::CaseInsensitive) || fn.endsWith(".html", Qt::CaseInsensitive)) )
    fn += ".odt"; // default
  setCurrentFileName(fn);
  return fileSave();
}

void TextEdit::filePrint()
{
#ifndef QT_NO_PRINTER
  QPrinter printer(QPrinter::HighResolution);
  QPrintDialog *dlg = new QPrintDialog(&printer, this);
  if (textEdit_->textCursor().hasSelection())
    dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
  dlg->setWindowTitle(tr("Print Document"));
  if (dlg->exec() == QDialog::Accepted) {
    textEdit_->print(&printer);
  }
  delete dlg;
#endif
}

void TextEdit::filePrintPreview()
{
#ifndef QT_NO_PRINTER
  QPrinter printer(QPrinter::HighResolution);
  QPrintPreviewDialog preview(&printer, this);
  connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
  preview.exec();
#endif
}

void TextEdit::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
  Q_UNUSED(printer)
#else
  textEdit_->print(printer);
#endif
}

void TextEdit::filePrintPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
  QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
                          QString(), "*.pdf");
  if (!fileName.isEmpty()) {
    if (QFileInfo(fileName).suffix().isEmpty())
      fileName.append(".pdf");
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    textEdit_->document()->print(&printer);
  }
//! [0]
#endif
}
*/

/*
void TextEdit::about()
{
  QMessageBox::about(this, tr("About"), tr("This example demonstrates Qt's "
    "rich text editing facilities in action, providing an example "
    "document for you to experiment with."));
}
*/

/*
void TextEdit::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
  if (const QMimeData *md = QApplication::clipboard()->mimeData())
    actionPaste->setEnabled(md->hasText());
#endif
}
*/

