// annotationbrowser.cc
// 10/3/2011

#include "annotationbrowser.h"
#include "annotationeditor.h"
#include "uistyle.h"
#include "stylesheet.h"
#include "logger.h"
#include "tr.h"
#include "filteredtableview.h"
#include "core/util/datetime.h"
#include "core/cloud/traits.h"
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

AnnotationBrowser::AnnotationBrowser(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), userId_(0)
{
  setWindowTitle(TR(T_TITLE_ANNOTATIONBROWSER));
  UiStyle::globalInstance()->setWindowStyle(this);
  setAcceptDrops(true);

  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setHeaderData(sourceModel_);
  tableView_ = new FilteredTableView(sourceModel_, this);

  editor_ = new AnnotationEditor(this);
  connect(editor_, SIGNAL(textSaved(QString)), SLOT(saveAnnotationText(QString)));

  createLayout();
  createActions();
}

void
AnnotationBrowser::createLayout()
{
  // Set layout

  QLayout *layout = new QStackedLayout; {
    layout->addWidget(tableView_);

    layout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(9, 9, 9, 9);

  } setLayout(layout);

  // Set initial states

  tableView_->sortByColumn(HD_CreateTime, Qt::DescendingOrder);
  tableView_->setCurrentColumn(HD_Text);
}

void
AnnotationBrowser::createActions()
{
#define MAKE_ACTION(_action, _styleid, _slot) \
  _action = new QAction(QIcon(RC_IMAGE_##_styleid), TR(T_MENUTEXT_##_styleid), this); \
  _action->setToolTip(TR(T_TOOLTIP_##_styleid)); \
  connect(_action, SIGNAL(triggered()), _slot);

  MAKE_ACTION(editAnnotAct_,  EDITTHISANNOT,  SLOT(editAnnotation()))
  MAKE_ACTION(copyAnnotAct_,  COPYTHISANNOT,  SLOT(copyAnnotation()))
  MAKE_ACTION(blessAnnotAct_, BLESSTHISANNOT, SLOT(blessAnnotation()))
  MAKE_ACTION(curseAnnotAct_, CURSETHISANNOT, SLOT(curseAnnotation()))
  MAKE_ACTION(blockAnnotAct_, BLOCKTHISANNOT, SLOT(blockAnnotation()))
  MAKE_ACTION(hideAnnotAct_,  HIDETHISANNOT,  SLOT(hideAnnotation()))
  MAKE_ACTION(showAnnotAct_,  SHOWTHISANNOT,  SLOT(showAnnotation()))

  MAKE_ACTION(viewUserAct_,   VIEWTHISUSER,   SLOT(viewUser()))
  MAKE_ACTION(blockUserAct_,  BLOCKTHISUSER,  SLOT(blockUser()))

#undef MAKE_ACTION

  // Create menus
  contextMenu_ = new QMenu(TR(T_TITLE_ANNOTATIONBROWSER), this);
  UiStyle::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true
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
  model->setHeaderData(HD_BlessedCount, Qt::Horizontal, TR(T_BLESSEDCOUNT));
  model->setHeaderData(HD_CursedCount, Qt::Horizontal, TR(T_CURSEDCOUNT));
  model->setHeaderData(HD_BlockedCount, Qt::Horizontal, TR(T_BLOCKEDCOUNT));
}

// - Properties -

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
  if (!index.isValid())
    return 0;
  bool ok;
  qint64 ret = index.data().toLongLong(&ok);
  if (!ok)
    return 0;

  return ret;
}

qint64
AnnotationBrowser::currentUserId() const
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


qint64
AnnotationBrowser::userId() const
{ return userId_; }

void
AnnotationBrowser::setUserId(qint64 uid)
{ userId_ = uid; }

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
#define FORMAT_TIME(_secs)        QDateTime::fromMSecsSinceEpoch(_secs * 1000)
#define FORMAT_POS(_msecs)        Core::msecs2time(_msecs)
#define FORMAT_LANGUAGE(_lang)    languageToString(_lang)
#define FORMAT_FLAGS(_flags)      annotationFlagsToStringList(_flags)
#define FORMAT_STATUS(_status)    annotationStatusToString(_status)

  sourceModel_->insertRow(0);


  sourceModel_->setData(sourceModel_->index(0, HD_Text), a.text(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Pos), FORMAT_POS(a.pos()), Qt::DisplayRole);
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
  sourceModel_->setData(sourceModel_->index(0, HD_BlessedCount), a.blessedCount(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_CursedCount), a.cursedCount(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_BlockedCount), a.blockedCount(), Qt::DisplayRole);

  for (int i = 0; i < HD_Count; i++)
    sourceModel_->setData(sourceModel_->index(0, i), sourceModel_->headerData(i, Qt::Horizontal), Qt::ToolTipRole);

#undef FORMAT_TIME
#undef FORMAT_POS
#undef FORMAT_LANGUAGE
#undef FORMAT_FLAGS
#undef FORMAT_STATUS
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

// - Formatter -

QString
AnnotationBrowser::languageToString(int lang)
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
AnnotationBrowser::annotationFlagsToStringList(int flags)
{
  QStringList ret;
  if (flags | Annotation::AF_Anonymous)
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
AnnotationBrowser::saveAnnotationText(const QString &text)
{
  if (!editedIndex_.isValid())
    return;

  if (currentUserId() != userId_)
    warn(tr("cannot edit other's annotation"));

  int row = editedIndex_.row();

  QModelIndex mi = editedIndex_.sibling(row, HD_Text);

  sourceModel_->setData(mi, text, Qt::DisplayRole);
  qint64 id = editor_->id();

  sourceModel_->setData(editedIndex_.sibling(row, HD_UpdateTime), QDateTime::currentDateTime(), Qt::DisplayRole);

  emit annotationTextUpdated(text, id);
}

void
AnnotationBrowser::editAnnotation()
{
  // TODO: CHECKPOINT
  qint64 id = currentId();
  if (!id)
    return;

  editedIndex_ = currentIndex();

  editor_->setId(id);
  editor_->setText(currentText());
  editor_->show();
}

void
AnnotationBrowser::copyAnnotation() {
  QString text = currentText();
  if (text.isEmpty())
    return;

  QClipboard *clipboard = QApplication::clipboard();
  if (clipboard) {
    clipboard->setText(text);
    log(TR(T_SUCCEED_ANNOTATION_COPIED) + ": " + text);
  } else
    warn(TR(T_ERROR_CLIPBOARD_UNAVAILABLE));
}

void
AnnotationBrowser::showAnnotation() { /* TODO */ }
void
AnnotationBrowser::hideAnnotation() { /* TODO */ }
void
AnnotationBrowser::blessAnnotation() { /* TODO */ }
void
AnnotationBrowser::curseAnnotation() { /* TODO */ }
void
AnnotationBrowser::blockAnnotation() { /* TODO */ }

void
AnnotationBrowser::viewUser() { /* TODO */ }
void
AnnotationBrowser::blockUser() { /* TODO */ }

// - Events -

// TODO: Dynamically compose menu content
void
AnnotationBrowser::contextMenuEvent(QContextMenuEvent *event)
{
  if (!event)
    return;
  if (!currentIndex().isValid())
    return;
  contextMenu_->clear();


  // Annotation
  if (currentIndex().isValid()) {
    // Edit
    {
    contextMenu_->addAction(editAnnotAct_);
    if (!currentText().isEmpty())
      contextMenu_->addAction(copyAnnotAct_);
    }

    // Cast: TODO
    if (currentId()) {
      //contextMenu_->addSeparator();

      //contextMenu_->addAction(blessAnnotAct_);
      //contextMenu_->addAction(curseAnnotAct_);
      //contextMenu_->addAction(hideAnnotAct_);
      //contextMenu_->addAction(blockAnnotAct_);
      //contextMenu_->addAction(showAnnotAct_);
    }
  }

  // User
  {
    contextMenu_->addSeparator();

    contextMenu_->addAction(viewUserAct_);
    contextMenu_->addAction(blockUserAct_);
  }

  // Pop up
  contextMenu_->popup(event->globalPos());
  event->accept();
}

void AnnotationBrowser::dragEnterEvent(QDragEnterEvent *event)     { emit dragEnterEventReceived(event); }
void AnnotationBrowser::dragMoveEvent(QDragMoveEvent *event)       { emit dragMoveEventReceived(event); }
void AnnotationBrowser::dragLeaveEvent(QDragLeaveEvent *event)     { emit dragLeaveEventReceived(event); }
void AnnotationBrowser::dropEvent(QDropEvent *event)               { emit dropEventReceived(event); }

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
*/

/*
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
  Q_UNUSED(printer);
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

