// mainwindow.cc
// 2/17/2012

#include "mainwindow.h"
#include "mainwindow_p.h"
#include "clipboardmonitor.h"
#include "global.h"
#include "rc.h"
#include "signer.h"
#include "settings.h"
#include "taskdialog.h"
#include "trayicon.h"
#include "project/common/acss.h"
#include "project/common/acui.h"
#include "project/common/acaboutdialog.h"
#include "project/common/acdownloader.h"
#include "project/common/acfilteredlistview.h"
#include "project/common/acplayer.h"
#include "module/qtext/datetime.h"
#include "module/download/downloader.h"
#include "module/download/downloadmanager.h"
#include "module/download/mrldownloadtask.h"
#include <QtGui>
#include <climits>

#define DEBUG "mainwindow"
#include "module/debug/debug.h"

#ifdef Q_OS_MAC
#  define K_CTRL        "cmd"
#else
#  define K_CTRL        "Ctrl"
#endif // Q_OS_MAC

enum { RefreshInterval = 3000 };
enum { MaxDownloadThreadCount = 3 };

// - Constructions -

//#define WINDOW_FLAGS (
//  Qt::CustomizeWindowHint |
//  Qt::WindowTitleHint |
//  Qt::WindowSystemMenuHint |
//  Qt::WindowMinMaxButtonsHint |
//  Qt::WindowCloseButtonHint )

MainWindow::MainWindow(QWidget *parent)
  : Base(parent), playerDelegate_(0), trayIcon_(0), taskDialog_(0)
{
  setWindowTitle(tr("Annot Downloader"));
  setWindowIcon(QIcon(RC_IMAGE_APP));

  downloadManager_ = new DownloadManager(this);
  downloadManager_->setMaxThreadCount(MaxDownloadThreadCount);

  createModels();
  createLayout();
  createActions();

  refreshTimer_ = new QTimer(this);
  refreshTimer_->setInterval(RefreshInterval);
  connect(refreshTimer_, SIGNAL(timeout()), SLOT(refresh()));

  connect(tableView_, SIGNAL(currentIndexChanged(QModelIndex)), SLOT(updateButtons()));

  clipboardMonitor_ = new ClipboardMonitor(this);
  connect(clipboardMonitor_, SIGNAL(message(QString)), SLOT(showMessage(QString)));
  connect(clipboardMonitor_, SIGNAL(warning(QString)), SLOT(warn(QString)));
  connect(clipboardMonitor_, SIGNAL(error(QString)), SLOT(error(QString)));
  connect(clipboardMonitor_, SIGNAL(notification(QString)), SLOT(notify(QString)));
  connect(clipboardMonitor_, SIGNAL(annotationUrlEntered(QString)), SLOT(promptUrl(QString)));
  connect(clipboardMonitor_, SIGNAL(mediaUrlEntered(QString)), SLOT(promptUrl(QString)));

  signer_ = new Signer(this);
  connect(signer_, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(signer_, SIGNAL(warning(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(signer_, SIGNAL(error(QString)), SLOT(error(QString)), Qt::QueuedConnection);
  connect(signer_, SIGNAL(notification(QString)), SLOT(notify(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(downloadFinished(QString,QString)), signer_, SLOT(signFileWithUrl(QString,QString)));

  DownloaderController *dc = DownloaderController::globalController();
  connect(dc, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(warning(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(error(QString)), SLOT(error(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(notification(QString)), SLOT(notify(QString)), Qt::QueuedConnection);

  foreach (const DownloadTaskInfo &t, Settings::globalSettings()->recentTasks())
    addTask(t);

  // - IPC -
  playerDelegate_ = new AcPlayer(this);

  appServer_ = new AcDownloaderServer(this);
  connect(appServer_, SIGNAL(arguments(QStringList)), SLOT(promptUrls(QStringList)), Qt::QueuedConnection);
  appServer_->start();

  if (QSystemTrayIcon::isSystemTrayAvailable()) {
    trayIcon_ = new TrayIcon(this, this);
    trayIcon_->show();
  }

  // - Post behaviors -
  tableView_->sortByColumn(HD_Id);
  tableView_->setFocus();
}

bool
MainWindow::isAddingUrls() const
{ return taskDialog_ && taskDialog_->isVisible(); }

void
MainWindow::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);
  ui->setStatusBarStyle(statusBar());

  startButton_ = ui->makeToolButton(AcUi::PushHint, tr("Start"), this, SLOT(start()));
  stopButton_ = ui->makeToolButton(AcUi::PushHint, tr("Stop"), this, SLOT(stop()));
  removeButton_ = ui->makeToolButton(AcUi::PushHint, tr("Remove"), this, SLOT(remove()));
  openButton_ = ui->makeToolButton(AcUi::PushHint, tr("Play"), this, SLOT(open()));
  openDirectoryButton_ = ui->makeToolButton(
        AcUi::PushHint, tr("Dir"), tr("Open directory"), this, SLOT(openDirectory()));
  addButton_ = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, tr("Add"), "", K_CTRL "+N", this, SLOT(add()));

  startButton_->setEnabled(false);
  stopButton_->setEnabled(false);
  removeButton_->setEnabled(false);
  openButton_->setEnabled(false);

  // Layout
  QVBoxLayout *rows = new QVBoxLayout; {
    QHBoxLayout *header = new QHBoxLayout,
                *footer = new QHBoxLayout;
    rows->addLayout(header);
    rows->addWidget(tableView_);
    rows->addLayout(footer);

    header->addWidget(startButton_);
    header->addWidget(stopButton_);
    header->addStretch();
    header->addWidget(removeButton_);

    footer->addWidget(openButton_);
    footer->addWidget(openDirectoryButton_);
    footer->addStretch();
    footer->addWidget(addButton_);

    int patch = 0;
    if (!AcUi::isAeroAvailable())
      patch = 9;

    // left, top, right, bottom
    header->setContentsMargins(0, 0, 0, 0);
    footer->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(9, patch, 9, patch);

  }
  QWidget *w = new QWidget;
  w->setLayout(rows);
  setCentralWidget(w);

#undef MAKE_BUTTON
}

void
MainWindow::createModels()
{
  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setHeaderData(sourceModel_);

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(sourceModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  }

  tableView_ = new AcFilteredListView(sourceModel_, proxyModel_, this);
}

void
MainWindow::createActions()
{
  startAct_ = new QAction(this); {
    startAct_->setText(tr("Start"));
    startAct_->setStatusTip(tr("Start"));
  } connect(startAct_, SIGNAL(triggered()), SLOT(start()));
  restartAct_ = new QAction(this); {
    restartAct_->setText(tr("Restart"));
    restartAct_->setStatusTip(tr("Restart"));
  } connect(restartAct_, SIGNAL(triggered()), SLOT(restart()));
  stopAct_ = new QAction(this); {
    stopAct_->setText(tr("Stop"));
    stopAct_->setStatusTip(tr("Stop"));
  } connect(stopAct_, SIGNAL(triggered()), SLOT(stop()));
  removeAct_ = new QAction(this); {
    removeAct_->setText(tr("Remove"));
    removeAct_->setStatusTip(tr("Remove"));
  } connect(removeAct_, SIGNAL(triggered()), SLOT(remove()));
  startAllAct_ = new QAction(this); {
    startAllAct_->setText(tr("Start all"));
    startAllAct_->setStatusTip(tr("Start all"));
  } connect(startAllAct_, SIGNAL(triggered()), SLOT(startAll()));
  stopAllAct_ = new QAction(this); {
    stopAllAct_->setText(tr("Stop all"));
    stopAllAct_->setStatusTip(tr("Stop all"));
  } connect(stopAllAct_, SIGNAL(triggered()), SLOT(stopAll()));
  removeAllAct_ = new QAction(this); {
    removeAllAct_->setText(tr("Remove all"));
    removeAllAct_->setStatusTip(tr("Remove all"));
  } connect(removeAllAct_, SIGNAL(triggered()), SLOT(removeAll()));
  openAct_ = new QAction(this); {
    openAct_->setText(tr("Play"));
    openAct_->setStatusTip(tr("Open with Annot Player"));
  } connect(openAct_, SIGNAL(triggered()), SLOT(open()));
  openDirectoryAct_ = new QAction(this); {
    openDirectoryAct_->setText(tr("Open directory"));
    openDirectoryAct_->setStatusTip(tr("Open directory"));
  } connect(openDirectoryAct_, SIGNAL(triggered()), SLOT(openDirectory()));
  hideAct_ = new QAction(this); {
    hideAct_->setText(tr("Hide"));
    hideAct_->setStatusTip(tr("Hide"));
  } connect(hideAct_, SIGNAL(triggered()), SLOT(hide()));
  quitAct_ = new QAction(this); {
    quitAct_->setText(tr("Quit"));
    quitAct_->setStatusTip(tr("Quit"));
    quitAct_->setShortcut(QKeySequence::Quit);
  } connect(quitAct_, SIGNAL(triggered()), SLOT(close()));
  newAct_ = new QAction(this); {
    newAct_->setText(tr("New"));
    newAct_->setStatusTip(tr("New"));
    newAct_->setShortcut(QKeySequence::New);
  } connect(newAct_, SIGNAL(triggered()), SLOT(add()));

  // Create menus
  contextMenu_ = new QMenu(this);
  AcUi::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true
  contextMenu_->addAction(startAct_);
  contextMenu_->addAction(openAct_);
  contextMenu_->addAction(openDirectoryAct_);
  contextMenu_->addSeparator();
  contextMenu_->addAction(stopAct_);
  contextMenu_->addAction(restartAct_);
  contextMenu_->addAction(removeAct_);
  contextMenu_->addSeparator();
  contextMenu_->addAction(startAllAct_);
  contextMenu_->addAction(stopAllAct_);
  contextMenu_->addAction(removeAllAct_);
#ifndef Q_WS_MAC
  contextMenu_->addSeparator();
  contextMenu_->addAction(hideAct_);
  contextMenu_->addAction(quitAct_);
#endif // Q_WS_MAC


  hideAct_->setShortcut(QKeySequence("Esc"));
  connect(new QShortcut(QKeySequence("Esc"), this), SIGNAL(activated()), SLOT(hide()));

  quitAct_->setShortcut(QKeySequence("CTRL+W"));
  connect(new QShortcut(QKeySequence("CTRL+W"), this), SIGNAL(activated()), SLOT(close()));

  connect(new QShortcut(QKeySequence::New, this), SIGNAL(activated()), SLOT(add()));

#ifdef Q_WS_WIN
  menuBar()->hide();
#endif // Q_WS_WIN
  QMenu *
  m = menuBar()->addMenu(tr("&File")); {
    m->addAction(newAct_);
    m->addSeparator();
    m->addAction(stopAllAct_);
    m->addAction(removeAllAct_);
    m->addSeparator();
    m->addAction(tr("Exit"), this, SLOT(close()), QKeySequence::Quit);
  }
  m = menuBar()->addMenu(tr("&Edit")); {
    m->addAction(startAct_);
    m->addAction(openAct_);
    m->addAction(openDirectoryAct_);
    m->addSeparator();
    m->addAction(stopAct_);
    m->addAction(restartAct_);
    m->addAction(removeAct_);
  }
  m = menuBar()->addMenu(tr("&Help")); {
    //helpMenu_->addAction(tr("&Help"), this, SLOT(help())); // DO NOT TRANSLATE ME
    m->addAction(tr("&About"), this, SLOT(about())); // DO NOT TRANSLATE ME
  }
}

// - Table -

void
MainWindow::setHeaderData(QAbstractItemModel *model)
{
  Q_ASSERT(model);
  model->setHeaderData(HD_Name, Qt::Horizontal, tr("Name"));
  model->setHeaderData(HD_State, Qt::Horizontal, tr("State"));
  model->setHeaderData(HD_Size, Qt::Horizontal, tr("Size"));
  model->setHeaderData(HD_Speed, Qt::Horizontal, tr("Speed"));
  model->setHeaderData(HD_Percentage, Qt::Horizontal, "%");
  model->setHeaderData(HD_RemainingTime, Qt::Horizontal, tr("Remaining time"));
  model->setHeaderData(HD_Path, Qt::Horizontal, tr("Path"));
  model->setHeaderData(HD_Url, Qt::Horizontal, tr("URL"));
  model->setHeaderData(HD_Id, Qt::Horizontal, tr("ID"));
}

void
MainWindow::clear()
{
  tableView_->clear();
  setHeaderData(sourceModel_);
}

int
MainWindow::currentId() const
{
  QModelIndex mi = tableView_->currentIndex();
  if (!mi.isValid())
    return 0;
  mi = mi.sibling(mi.row(), HD_Id);
  if (!mi.isValid())
    return 0;
  bool ok;
  int ret = mi.data().toInt(&ok);
  if (!ok)
    return 0;
  return ret;
}

DownloadTask*
MainWindow::currentTask() const
{
  int id = currentId();
  if (id <= 0)
    return 0;
  return downloadManager_->taskWithId(id);
}

// - Actions -

void
MainWindow::checkClipboard()
{ clipboardMonitor_->invalidateClipboard(); }

TaskDialog*
MainWindow::taskDialog()
{
  if (!taskDialog_) {
    taskDialog_ = new TaskDialog(this);
    connect(taskDialog_, SIGNAL(urlsAdded(QStringList,bool)), SLOT(addUrls(QStringList,bool)));
    connect(taskDialog_, SIGNAL(urlsAdded(QStringList,bool)), downloadManager_, SLOT(refreshSchedule()));
    connect(taskDialog_, SIGNAL(message(QString)), SLOT(showMessage(QString)));
    connect(taskDialog_, SIGNAL(warning(QString)), SLOT(warn(QString)));
    connect(taskDialog_, SIGNAL(error(QString)), SLOT(error(QString)));
  }
  return taskDialog_;
}

void
MainWindow::promptUrls(const QStringList &urls)
{
  DOUT("enter: urls =" << urls);
  foreach (QString text, urls)
    promptUrl(text);
  DOUT("exit");
}

void
MainWindow::promptUrl(const QString &text)
{
  TaskDialog *dlg = taskDialog();
  if (dlg->isVisible())
    dlg->addText(text);
  else {
    dlg->setText(text);
    dlg->show();
  }
}

void
MainWindow::add()
{ taskDialog()->show(); }

void
MainWindow::start()
{
  DownloadTask *t = currentTask();
  if (t && !t->isRunning() && !t->isFinished()) {
    t->stop();
    t->start();
  }
}

void
MainWindow::restart()
{
  DownloadTask *t = currentTask();
  if (t) {
    t->stop();
    t->start();
  }
}

void
MainWindow::stop()
{
  DownloadTask *t = currentTask();
  if (t && t->isRunning())
    t->stop();
}

void
MainWindow::startAll()
{
  foreach (DownloadTask *t, downloadManager_->tasks())
    if (t->isStopped())
      t->setState(DownloadTask::Pending);
  downloadManager_->refreshSchedule();
}

void
MainWindow::stopAll()
{
  foreach (DownloadTask *t, downloadManager_->tasks())
    if (!t->isFinished())
      t->stop();
}

void
MainWindow::removeAll()
{
  clear();
  downloadManager_->removeAll();
}

void
MainWindow::remove()
{
  DownloadTask *t = currentTask();
  if (t)
    downloadManager_->removeTask(t);
  tableView_->removeCurrentRow();
}

void
MainWindow::open()
{
  DownloadTask *t = currentTask();
  if (t && !t->fileName().isEmpty())
    playerDelegate_->openUrl(t->fileName());
}

void
MainWindow::openDirectory()
{
  QString url = G_PATH_DOWNLOADS;
#ifdef Q_OS_WIN
  url.replace('\\', '/');
  url.prepend('/');
#endif // Q_OS_WIN
  url.prepend("file://");
  QDesktopServices::openUrl(url);
}

// - Download -

void
MainWindow::addUrls(const QStringList &urls, bool batch)
{
  Q_UNUSED(batch);
  foreach (QString url, urls)
    addUrl(url);
}

void
MainWindow::addUrl(const QString &url)
{
  DOUT("enter: url =" << url);

  if (downloadManager_->taskWithUrl(url)) {
    showMessage(tr("task exists") + ": " + url);
    return;
  }

  DownloadTask *t = new MrlDownloadTask(url, downloadManager_);
  t->setState(DownloadTask::Pending);
  addTask(t);

  DOUT("exit");
}

void
MainWindow::addTask(const DownloadTaskInfo &info)
{ addTask(new MrlDownloadTask(info, downloadManager_)); }

void
MainWindow::addTask(DownloadTask *t)
{
  Q_ASSERT(t);
  t->setAutoDelete(false);
  t->setDownloadPath(G_PATH_DOWNLOADS);
  connect(t, SIGNAL(error(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(t, SIGNAL(finished(DownloadTask*)), SLOT(finish(DownloadTask*)), Qt::QueuedConnection);
  downloadManager_->addTask(t);

#define FORMAT_TIME(_msecs)       downloadTimeToString(_msecs)
#define FORMAT_STATE(_state)      downloadStateToString(_state)
#define FORMAT_PERCENTAGE(_real)  QString::number((_real)*100, 'f', 2) + "%"
#define FORMAT_SIZE(_size)        downloadSizeToString(_size)
#define FORMAT_SPEED(_speed)      downloadSpeedToString(_speed)

  sourceModel_->insertRow(0);

  sourceModel_->setData(sourceModel_->index(0, HD_Name), "-", Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_State), FORMAT_STATE(t->state()), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Size), "-", Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Speed), "-", Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Percentage), FORMAT_PERCENTAGE(0.0), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_RemainingTime), "-", Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Path), "-", Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Url), t->url(), Qt::DisplayRole);
  sourceModel_->setData(sourceModel_->index(0, HD_Id), t->id(), Qt::DisplayRole);

  tableView_->updateCount();
#undef FORMAT_TIME
#undef FORMAT_STATE
#undef FORMAT_PERCENTAGE
#undef FORMAT_SIZE
#undef FORMAT_SPEED
}

void
MainWindow::refresh()
{
  if (downloadManager_->isEmpty()) {
    setWindowTitle(tr("Annot Downloader"));
    return;
  }

#define FORMAT_TIME(_msecs)       downloadTimeToString(_msecs)
#define FORMAT_STATE(_state)      downloadStateToString(_state)
#define FORMAT_PERCENTAGE(_real)  QString::number((_real)*100, 'f', 2) + "%"
#define FORMAT_SIZE(_size)        downloadSizeToString(_size)
#define FORMAT_SPEED(_speed)      downloadSpeedToString(_speed)

  qreal totalSpeed = 0;
  qreal leastPercentage = 0;
  qint64 totalRemainingTime = 0;

  for (int row = 0; row < sourceModel_->rowCount(); row++) {
    QModelIndex index = sourceModel_->index(row, HD_Id);
    if (!index.isValid())
      continue;
    bool ok;
    int id = index.data().toInt(&ok);
    if (!ok)
      continue;
    DownloadTask *t = downloadManager_->taskWithId(id);
    if (!t)
      continue;

    QString title = t->title();
    if (title.isEmpty())
      title = t->url();
    sourceModel_->setData(sourceModel_->index(row, HD_Name), title, Qt::DisplayRole);
    sourceModel_->setData(sourceModel_->index(row, HD_State), FORMAT_STATE(t->state()), Qt::DisplayRole);
    sourceModel_->setData(sourceModel_->index(row, HD_Size), FORMAT_SIZE(t->size()), Qt::DisplayRole);
    sourceModel_->setData(sourceModel_->index(row, HD_Speed), t->isFinished() ? QString("-") : FORMAT_SPEED(t->speed()), Qt::DisplayRole);
    sourceModel_->setData(sourceModel_->index(row, HD_Percentage), FORMAT_PERCENTAGE(t->percentage()), Qt::DisplayRole);
    sourceModel_->setData(sourceModel_->index(row, HD_RemainingTime), t->isFinished() ? QString("-") : FORMAT_TIME(t->remainingTime()), Qt::DisplayRole);
    sourceModel_->setData(sourceModel_->index(row, HD_Path), t->fileName(), Qt::DisplayRole);
    sourceModel_->setData(sourceModel_->index(row, HD_Url), t->url(), Qt::DisplayRole);
    //sourceModel_->setData(sourceModel_->index(row, HD_Id), t->id(), Qt::DisplayRole);

    if (t->isRunning()) {
      totalRemainingTime += t->remainingTime();
      leastPercentage = qMin(leastPercentage, t->percentage());
    }

    if (t->isRunning())
      totalSpeed += t->speed();
  }

  tableView_->updateCount();
  updateButtons();

  QString title;
  if (totalSpeed > 0) {
    title += " (";
    title += FORMAT_SPEED(totalSpeed);
    if (leastPercentage)
      title += ", " + FORMAT_PERCENTAGE(leastPercentage);
    if (totalRemainingTime) {
      QString ts = FORMAT_TIME(totalRemainingTime);
      if (!ts.isEmpty())
        title += ", " + ts;
    }
    title += ")";
  }
  if (title.isEmpty())
   title = tr("Annot Downloader");
  else
   title.prepend(tr("Annot Downloader"));

  setWindowTitle(title);
  if (trayIcon_)
    trayIcon_->setToolTip(title);
#undef FORMAT_TIME
#undef FORMAT_STATE
#undef FORMAT_PERCENTAGE
#undef FORMAT_SIZE
#undef FORMAT_SPEED
}

// - Events -

void
MainWindow::finish(DownloadTask *task)
{
  Q_ASSERT(task);
  if (!task->isFinished())
    return;
  QString file = task->fileName();
  QString title;
  if (QFile::exists(file)) {
    title = tr("download finished");
    QString msg = title + ": " + file;
    showMessage(msg);
    emit downloadFinished(file, task->url());
  } else {
    title = tr("download failed");
    QString msg = title + ": " + file;
    warn(msg);
  }
  QApplication::beep();
  if (trayIcon_)
    trayIcon_->showMessage(title, file);
  AcLocationManager::globalInstance()->createDownloadsLocation();
}

void
MainWindow::updateButtons()
{
  DownloadTask *t = currentTask();
  if (!t) {
    startButton_->setEnabled(false);
    stopButton_->setEnabled(false);
    removeButton_->setEnabled(false);
    openButton_->setEnabled(false);
  } else {
    bool e = QFile::exists(t->fileName());
    startButton_->setEnabled(!t->isRunning() && !e);
    stopButton_->setEnabled(!t->isStopped());
    removeButton_->setEnabled(true);
    openButton_->setEnabled(t->isFinished());
  }
}

void
MainWindow::updateActions()
{
  DownloadTask *t = currentTask();
  if (!t) {
    startAct_->setEnabled(false);
    restartAct_->setEnabled(false);
    stopAct_->setEnabled(false);
    removeAct_->setEnabled(false);
    openAct_->setEnabled(false);
  } else {
    bool e = QFile::exists(t->fileName());
    startAct_->setEnabled(!t->isRunning() && !e);
    restartAct_->setEnabled(true);
    stopAct_->setEnabled(!t->isStopped());
    removeAct_->setEnabled(true);
    openAct_->setEnabled(t->isFinished());
  }
}

void
MainWindow::setVisible(bool visible)
{
  if (visible) {
    refresh();
    refreshTimer_->start();
  } else
    refreshTimer_->stop();
  Base::setVisible(visible);
}

// - Helper -

QString
MainWindow::downloadStateToString(int state) const
{
  // enum State { Error = -1, Stopped = 0, Downloading = 1, Pending, Finished };
  switch (state) {
  case DownloadTask::Stopped:   return tr("Stopped");
  case DownloadTask::Downloading: return tr("Downloading");
  case DownloadTask::Pending:   return tr("Pending");
  case DownloadTask::Finished:  return tr("Finished");
  case DownloadTask::Error:     return tr("Error");
  default: Q_ASSERT(0);         return tr("Error");
  }
}

QString
MainWindow::downloadSizeToString(qint64 size) const
{
  if (size < 1024)
    return QString::number(size) + " B";
  else if (size < 1024 * 1024)
    return QString::number(size / 1014) + " KB";
  else
    return QString::number(size / (1024.0 * 1024), 'f', 1) + " MB";
}

QString
MainWindow::downloadSpeedToString(qreal speed) const
{
  if (speed < 1024)
    return QString::number((int)speed) + " B/s";
  else if (speed < 1024 * 1024)
    return QString::number((int)speed / 1024) + " KB/s";
  else
    return QString::number(speed / (1024.0 * 1024), 'f', 2) + " MB/s";
}

QString
MainWindow::downloadTimeToString(qint64 msecs) const
{
  QString ret = QtExt::msecs2time(msecs).toString();
  if (ret.isEmpty())
    ret = "-";
  return ret;
}

// - Events -

void
MainWindow::closeEvent(QCloseEvent *event)
{
  enum { CloseTimeout = 3000 };

  DOUT("enter");
  appServer_->stop();

  hide();
  if (trayIcon_)
    trayIcon_->hide();

  if (taskDialog_)
    taskDialog_->hide();

  Settings *settings = Settings::globalSettings();
  settings->setRecentTasks(downloadManager_->tasks().infoList());
  settings->setRecentSize(size());

  settings->sync();

  downloadManager_->stopAll();

  signer_->dispose();

  DownloaderController::globalController()->abort();

  if (QThreadPool::globalInstance()->activeThreadCount()) {
#if QT_VERSION >= 0x040800
    // wait for at most 2 seconds ant kill all threads
    QThreadPool::globalInstance()->waitForDone(CloseTimeout);
#else
    //DOUT("WARNING: killing active threads; will be fixed in Qt 4.8");
    QThreadPool::globalInstance()->waitForDone();
#endif  // QT_VERSION
  }

#ifdef Q_WS_WIN
  QTimer::singleShot(0, qApp, SLOT(quit())); // ensure quit app and clean up zombie threads
#endif // Q_WS_WIN

  Base::closeEvent(event);
  DOUT("exit");
}

bool
MainWindow::event(QEvent *e)
{
  bool accept = true;
  switch (e->type()) {
  case QEvent::FileOpen: // See: http://www.qtcentre.org/wiki/index.php?title=Opening_documents_in_the_Mac_OS_X_Finder
    {
      QFileOpenEvent *fe = static_cast<QFileOpenEvent *>(e);
      Q_ASSERT(fe);
      QString url = fe->url().toString();
      if (!url.isEmpty())
        QTimer::singleShot(0, new slot_::PromptUrl(url, this), SLOT(trigger()));
    } break;
  default: accept = Base::event(e);
  }
  return accept;
}

void
MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);
  updateActions();
  contextMenu_->exec(event->globalPos());
  event->accept();
}

void
MainWindow::about()
{
  static AcAboutDialog *w = 0;
  if (!w)
    w = new AcAboutDialog(G_APPLICATION, G_VERSION, this);
  w->show();
}


// EOF
