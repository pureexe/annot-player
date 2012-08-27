// annotationlistview.cc
// 10/3/2011

#include "annotationlistview.h"
#include "annotationlistmodel.h"
#include "annotationeditor.h"
#include "tr.h"
#include "signalhub.h"
#include "project/common/acfilteredtableview.h"
#include "project/common/acui.h"
#include "module/qtext/datetime.h"
#include "module/qtext/layoutwidget.h"
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

AnnotationListView::AnnotationListView(SignalHub *hub, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), hub_(hub),
    userId_(0), annotationPos_(0), editor_(0), contextMenu_(0)
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
  tableView_->sortByColumn(AnnotationListModel::HD_CreateTime, Qt::DescendingOrder);
  tableView_->setCurrentColumn(AnnotationListModel::HD_Text);
}

void
AnnotationListView::createModel()
{
  sourceModel_ = new AnnotationListModel(hub_, this);

  filterMeModel_ = new QSortFilterProxyModel; {
    filterMeModel_->setDynamicSortFilter(true);
    filterMeModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
    filterMeModel_->setFilterKeyColumn(AnnotationListModel::HD_UserId);

    filterMeModel_->setSourceModel(sourceModel_);
  }
  filterNowModel_ = new QSortFilterProxyModel; {
    filterNowModel_->setDynamicSortFilter(true);
    filterNowModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
    filterNowModel_->setFilterKeyColumn(AnnotationListModel::HD_Pos);

    filterNowModel_->setSourceModel(filterMeModel_);
  }
  filterSubtitleModel_ = new QSortFilterProxyModel; {
    filterSubtitleModel_->setDynamicSortFilter(true);
    filterSubtitleModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
    filterSubtitleModel_->setFilterKeyColumn(AnnotationListModel::HD_Text);

    filterSubtitleModel_->setSourceModel(filterNowModel_);
  }

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);

    proxyModel_->setSourceModel(filterSubtitleModel_);
  }

  tableView_ = new AcFilteredTableView(sourceModel_, proxyModel_, this);
  connect(tableView_->view(), SIGNAL(doubleClicked(QModelIndex)), SLOT(editAnnotation()));
}

void
AnnotationListView::createLayout()
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
}

void
AnnotationListView::createContextMenu()
{
  // Create menus
  contextMenu_ = new QMenu(this);
  AcUi::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true
  editAnnotAct_ = contextMenu_->addAction(TR(T_EDIT) + " [" + tr("DoubleClick") + "]", this, SLOT(editAnnotation()));
  copyAnnotAct_ = contextMenu_->addAction(TR(T_COPY), this, SLOT(copyAnnotation()));
  contextMenu_->addSeparator();
  deleteAnnotAct_ = contextMenu_->addAction(TR(T_DELETE), this, SLOT(deleteAnnotation()));
  blessAnnotAct_ = contextMenu_->addAction(TR(T_BLESS), this, SLOT(blessAnnotation()));
  curseAnnotAct_ = contextMenu_->addAction(TR(T_CURSE), this, SLOT(curseAnnotation()));
  blockAnnotAct_ = contextMenu_->addAction(TR(T_MENUTEXT_BLOCKTHISANNOT), this, SLOT(blockAnnotation()));
  //contextMenu_->addSeparator();
  //viewUserAct_ = contextMenu_->addAction(TR(T_MENUTEXT_VIEWTHISUSER), this, SLOT(viewUser()));
  blockUserAct_ = contextMenu_->addAction(TR(T_MENUTEXT_BLOCKTHISUSER), this, SLOT(blockUser()));

  contextMenu_->addSeparator();
  analyzeUserAct_ = contextMenu_->addAction(tr("User Analytics"), this, SLOT(analyzeUser()));
  analyticsAct_ = contextMenu_->addAction(tr("Annotation Analytics"), this, SIGNAL(annotationAnalyticsRequested()));
}

// - Properties -

AnnotationEditor*
AnnotationListView::editor() const
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
AnnotationListView::currentIndex() const
{ return tableView_->currentIndex(); }

qint64
AnnotationListView::currentId() const
{ return sourceModel_->indexId(currentIndex()); }

qint64
AnnotationListView::currentUserId() const
{ return sourceModel_->indexUserId(currentIndex()); }

QString
AnnotationListView::currentUserAlias() const
{ return sourceModel_->indexUserAlias(currentIndex()); }

QString
AnnotationListView::currentText() const
{ return sourceModel_->indexText(currentIndex()); }

void
AnnotationListView::setAnnotations(const AnnotationList &l)
{
  clear();
  addAnnotations(l);
}

bool
AnnotationListView::isEmpty() const
{ return sourceModel_->isEmpty(); }

void
AnnotationListView::addAnnotation(const Annotation &a)
{
  sourceModel_->addAnnotation(a);
  if (isVisible())
    tableView_->updateCount();
}

void
AnnotationListView::addAnnotations(const AnnotationList &l)
{
  if (!l.isEmpty()) {
    foreach (const Annotation &a, l)
      //addAnnotation(a);
      sourceModel_->addAnnotation(a);
    if (isVisible())
      tableView_->updateCount();
  }
}

void
AnnotationListView::removeAnnotations()
{ tableView_->clear(); }

void
AnnotationListView::clear()
{ removeAnnotations(); }

void
AnnotationListView::removeAnnotationWithId(qint64 id)
{
  int row = sourceModel_->rowById(id);
  if (row >= 0) {
    sourceModel_->removeRow(row);
    if (isVisible())
      tableView_->updateCount();
  }
}

// - Actions -

void
AnnotationListView::takeAnnotationOwnership()
{
  QModelIndex index = currentIndex();
  if (!index.isValid())
    return;

  if (!userId_ || currentUserId() != User::guest().id()) {
    emit warning(tr("cannot modify other's annotation"));
    return;
  }

  int row = index.row();
  QModelIndex mi = index.sibling(row, AnnotationListModel::HD_UserId);
  sourceModel_->setData(mi, userId_, Qt::DisplayRole);

  qint64 id = currentId();
  if (id) {
    updateAnnotationUserIdWithId(userId_, id);
    emit annotationUserIdUpdatedWithId(userId_, id);
  }
}

void
AnnotationListView::saveAnnotationText(const QString &text)
{
  if (!editedIndex_.isValid())
    return;

  if (!userId_ || currentUserId() != userId_) {
    emit warning(tr("cannot edit other's annotation"));
    return;
  }

  int row = editedIndex_.row();

  QModelIndex mi = editedIndex_.sibling(row, AnnotationListModel::HD_Text);

  sourceModel_->setData(mi, text, Qt::DisplayRole);
  qint64 id = editor()->id();

  if (id) {
    updateAnnotationTextWithId(text, id);
    emit annotationTextUpdatedWithId(text, id);
  }
}

void
AnnotationListView::updateAnnotationTextWithId(const QString &text, qint64 id)
{ sourceModel_->updateAnnotationTextWithId(text, id); }

void
AnnotationListView::updateAnnotationUserIdWithId(qint64 userId, qint64 id)
{ sourceModel_->updateAnnotationUserIdWithId(userId, id); }

void
AnnotationListView::editAnnotation()
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
AnnotationListView::copyAnnotation()
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
AnnotationListView::deleteAnnotation()
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
AnnotationListView::blessAnnotation()
{
  qint64 id = currentId();
  if (id)
    emit annotationBlessedWithId(id);
  qint64 uid = currentUserId();
  if (uid)
    emit userBlessedWithId(uid);
}

void
AnnotationListView::curseAnnotation()
{
  qint64 id = currentId();
  if (id)
    emit annotationCursedWithId(id);
  qint64 uid = currentUserId();
  if (uid)
    emit userCursedWithId(uid);
}

void
AnnotationListView::blockAnnotation()
{
  QString text = currentText();
  if (!text.isEmpty())
    emit annotationBlockedWithText(text);
  qint64 id = currentId();
  if (id)
    emit annotationBlockedWithId(id);
}

void
AnnotationListView::viewUser()
{
  // CHECKPOINT
}

void
AnnotationListView::blockUser()
{
  QString text = currentUserAlias();
  if (!text.isEmpty())
    emit userBlockedWithAlias(text);
  qint64 uid = currentUserId();
  if (uid)
    emit userBlockedWithId(uid);
}

void
AnnotationListView::analyzeUser()
{
  qint64 uid = currentUserId();
  if (uid)
    emit userAnalyticsRequested(uid);
}

// - Events -

void
AnnotationListView::setVisible(bool visible)
{
  if (visible) {
    invalidateFilters();
    tableView_->updateCount();
  }
  Base::setVisible(visible);
}

void
AnnotationListView::contextMenuEvent(QContextMenuEvent *event)
{
  if (!currentIndex().isValid())
    return;
  updateContextMenu();
  contextMenu_->popup(event->globalPos());
  event->accept();
}

void
AnnotationListView::updateContextMenu()
{
  if (!contextMenu_)
    createContextMenu();
  //editAnnotAct_->setVisible(true);
  copyAnnotAct_->setVisible(!currentText().isEmpty());

  qint64 id = currentId();
  qint64 cuid = currentUserId();

  deleteAnnotAct_->setVisible(id && userId_ == cuid);
  //blessAnnotAct_->setVisible(id > 0 && userId_ != cuid);
  //curseAnnotAct_->setVisible(id > 0 && userId_ != cuid);
  blessAnnotAct_->setVisible(false);
  curseAnnotAct_->setVisible(false);
  blockAnnotAct_->setVisible(id && userId_ != cuid);
  //viewUserAct_->setVisible(cuid && cuid != userId_);
  blockUserAct_->setVisible(cuid && cuid != userId_);
  analyzeUserAct_->setVisible(cuid);
  analyticsAct_->setVisible(true);
}

//void AnnotationListView::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
//void AnnotationListView::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
//void AnnotationListView::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
//void AnnotationListView::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

// - Filters -

void
AnnotationListView::setAnnotationPos(qint64 pos)
{
  annotationPos_ = pos;
  if (isVisible())
    invalidateFilters();
}

void
AnnotationListView::setNow(bool t)
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
AnnotationListView::setMe(bool t)
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
AnnotationListView::setSubtitle(bool t)
{
  QString s;
  if (t)
    s = CORE_CMD_SUB;
  filterSubtitleModel_->setFilterFixedString(s);
  tableView_->updateCount();
}

void
AnnotationListView::invalidateFilters()
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

