// mainwindow.cc
// 6/18/2011

#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "documentwidget.h"
#include "maintabwidget.h"
#include <QtGui>

#define PATH_OPENFILE_DEFAULT (QDir::homePath())

#define FILEFILTER_DOCUMENT "( *.pdf *.ps);;"

/**
 * Constructionts
 */

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
    //ui(new Ui::MainWindow)
{
  //ui->setupUi(this);
  setWindowTitle(tr("Core PDF"));

  createDocuments();
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createTabs();
  createDockWindows();
 }

MainWindow::~MainWindow()
{
  //delete ui;
}

void
MainWindow::createTabs()
{
  //connect(tabWidget_, SIGNAL(currentChanged(int)),
  //    SLOT(currentPageChanged(int)));

  //vboxLayout->addWidget(tabWidget_);
  //QWidget *centerWidget = new QWidget(this);
  //centerWidget->setLayout(vboxLayout);
  //setCentralWidget(centerWidget);

  tabWidget_ = new MainTabWidget(this);
  setCentralWidget(tabWidget_);
  connect(tabWidget_, SIGNAL(newTabRequested()), SLOT(newTab()));
  connect(tabWidget_, SIGNAL(closeAllRequested()), SLOT(closeAll()));
}

void
MainWindow::createDocuments()
{ }

void
MainWindow::createActions()
{
  //newAct = new QAction(QIcon(":/image/new.png"), tr("&New"), this);
  //newAct->setShortcuts(QKeySequence::New);
  //newAct->setStatusTip(tr("Create a new file"));
  //connect(newAct, SIGNAL(triggered()), SLOT(newFile()));

  openAct = new QAction(QIcon(":/image/open.png"), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open a document"));
  connect(openAct, SIGNAL(triggered()), SLOT(openFile()));
  /*
  saveAct = new QAction(QIcon(":/image/save.png"), tr("&Save..."), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save the current file"));
  connect(saveAct, SIGNAL(triggered()), SLOT(saveFile()));
  */

  //printAct = new QAction(QIcon(":/image/print.png"), tr("&Print..."), this);
  //printAct->setShortcuts(QKeySequence::Print);
  //printAct->setStatusTip(tr("Print the current file"));
  //connect(printAct, SIGNAL(triggered()), SLOT(print()));

  //undoAct = new QAction(QIcon(":/image/undo.png"), tr("&Undo"), this);
  //undoAct->setShortcuts(QKeySequence::Undo);
  //undoAct->setStatusTip(tr("Undo the last editing action"));
  //connect(undoAct, SIGNAL(triggered()), SLOT(undo()));

  //insertAct = new QAction(QIcon(":/image/insert.png"), tr("&Insert..."), this);
  //insertAct->setShortcuts(QKeySequence::UnknownKey);
  //insertAct->setStatusTip(tr("Insert trace at cursor"));
  //connect(insertAct, SIGNAL(triggered()), SLOT(insert()));

  quitAct = new QAction(tr("&Quit"), this);
  quitAct->setShortcuts(QKeySequence::Quit);
  quitAct->setStatusTip(tr("Quit the application"));
  connect(quitAct, SIGNAL(triggered()), SLOT(close()));

  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("About us"));
  connect(aboutAct, SIGNAL(triggered()), SLOT(about()));
}

void
MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  //fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  //fileMenu->addAction(saveAct);
  //fileMenu->addAction(printAct);
  //fileMenu->addSeparator();
  fileMenu->addAction(quitAct);

  //editMenu = menuBar()->addMenu(tr("&Edit"));
  //editMenu->addAction(undoAct);
  //editMenu->addAction(insertAct);
  //editMenu->addAction(analyzeAct);
  //editMenu->addAction(transformAct);
  //editMenu->addAction(applyAct);
  //editMenu->addAction(generateAct);
  //editMenu->addAction(clearAct);

  //viewMenu = menuBar()->addMenu(tr("&View"));

  windowMenu = menuBar()->addMenu(tr("&Window"));

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
}

void
MainWindow::createToolBars()
{
  //fileToolBar = addToolBar(tr("File"));
  //fileToolBar->addAction(newAct);
  //fileToolBar->addAction(openAct);
  //fileToolBar->addAction(saveAct);
  //fileToolBar->addAction(printAct);

  //editToolBar = addToolBar(tr("Edit"));
  //editToolBar->addAction(undoAct);
  //editToolBar->addAction(insertAct);
  //editToolBar->addAction(analyzeAct);
  //editToolBar->addAction(transformAct);
  //editToolBar->addAction(applyAct);
  //editToolBar->addAction(generateAct);
  //editToolBar->addAction(clearAct);
}

void
MainWindow::createStatusBar()
{
  statusBar()->showMessage(tr("Ready"));
}

void
MainWindow::createDockWindows()
{
  //setTabShape(QTabWidget::Rounded);
  //setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
  //setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);
}

/**
 * Slots
 */

void
MainWindow::newTab()
{ openFile(); }

void
MainWindow::closeAll()
{ close(); }

void
MainWindow::openFile()
{
  static QString last_path = PATH_OPENFILE_DEFAULT;

  QString title = tr("Open document");
  QString filters = tr("Documents") + FILEFILTER_DOCUMENT
                  + tr("All files") + "(*)";
  QString *selectedFilter = nullptr;

  QStringList l = QFileDialog::getOpenFileNames(
        this, title, last_path, filters, selectedFilter);

  if (!l.empty())
    last_path = QFileInfo(l.back()).absolutePath();

  foreach (const QString &fileName, l)
    openFile(fileName);
}

void
MainWindow::about()
{ QMessageBox::about(this, tr("About us"), tr("Core.Document")); }

/**
 * Actions:
 */

bool
MainWindow::openFile(const QString &filePath)
{
  // MainWindow > Dock Layou > ScrollArea > Document
  DocumentWidget *document = new DocumentWidget(filePath, this);
  if (document->bad()) {
    QTimer::singleShot(0, document, SLOT(deleteLater()));
    return false;
  }

  if (tabWidget_) {
    int index = tabWidget_->addTab(document, document->windowTitle());
    if (index >= 0)
      tabWidget_->setCurrentIndex(index);
  }
  /*
  QDockWidget *dock = new QDockWidget(this);
  dock->setWidget(document);
  dock->setWindowTitle(document->windowTitle());
  //dock->setTitleBarWidget(new QWidget(dock));
  //dock->setFeatures(QDockWidget::NoDockWidgetFeatures
  //                  | QDockWidget::DockWidgetClosable
  //                  | QDockWidget::DockWidgetMovable
  //                  | QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::TopDockWidgetArea, dock);
  windowMenu->addAction(dock->toggleViewAction());

  if (!documents_.empty()) {
    QDockWidget *last = dynamic_cast<QDockWidget*>(documents_.back()->parent());
    if (last) {
      tabifyDockWidget(last, dock);
      //dock->raise(); // no effect
    }
  }

  documents_ << document;
  */


  //DocumentWidget *document = new DocumentWidget(scrollArea);
  //QToolBar *toolBar = new QToolBar(tr("Document"), dock);


  //scrollArea->setWidget(document);
  //dock->setWidget(scrollArea);
  //dock->setTitleBarWidget(toolBar);

  //QLayout *layout = new QVBoxLayout(dock);
  //layout->addWidget(toolBar);
  //layout->addWidget(scrollArea);
  //dock->setLayout(layout);

  //QSize dockInitSize = QRect(QPoint(), document->size()).intersect(
  //                     QRect(QPoint(), DEFAULT_DOCUMENT_SIZE)).size();

  //dock->setMinimumSize(dockInitSize);
  //dock->setWindowTitle(document->windowTitle());
  //addDockWidget(Qt::TopDockWidgetArea, dock);

  //documents[document] = DocumentProperty(dock->toggleViewAction());
  //connect(document, SIGNAL(closed(DocumentWidget*)),
  //        SLOT(remove(DocumentWidget*)));

  return true;
}

/**
 * Events
 */

void
MainWindow::mousePressEvent(QMouseEvent *event)
{
  if (event && event->button() == Qt::LeftButton) {
    dragPos_ = event->globalPos() - frameGeometry().topLeft();
    event->accept();
  }
}

void
MainWindow::mouseMoveEvent(QMouseEvent *event)
{
  if (event && event->buttons() & Qt::LeftButton) {
    QPoint newPos = event->globalPos() - dragPos_;
    move(newPos);
    event->accept();
  }
}

void
MainWindow::mouseReleaseEvent(QMouseEvent *event)
{ Q_UNUSED(event) }

// EOF
