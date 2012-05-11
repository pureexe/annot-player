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
#include "project/common/acabout.h"
#include "project/common/acdownloader.h"
#include "project/common/acfilteredlistview.h"
#include "project/common/acbrowser.h"
#include "project/common/acplayer.h"
#include "project/common/acpreferences.h"
#include "module/animation/fadeanimation.h"
#include "module/qtext/datetime.h"
#include "module/qtext/layoutwidget.h"
#include "module/download/downloader.h"
#include "module/downloadtask/downloadmanager.h"
#include "module/downloadtask/mrldownloadtask.h"
#include <QtNetwork/QNetworkAccessManager>
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
  : Base(parent),
    about_(0), preferences_(0),
    trayIcon_(0), taskDialog_(0)
{
  setWindowTitle(tr("Annot Downloader"));
  setWindowIcon(QIcon(RC_IMAGE_APP));

  downloadManager_ = new DownloadManager(this);
  downloadManager_->setMaxThreadCount(MaxDownloadThreadCount);

  createModels();
  createSearchEngines();
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
  //connect(this, SIGNAL(downloadFinished(QString,QString)), signer_, SLOT(signFileWithUrl(QString,QString)));

  DownloaderController *dc = DownloaderController::globalController();
  connect(dc, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(warning(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(error(QString)), SLOT(error(QString)), Qt::QueuedConnection);
  connect(dc, SIGNAL(notification(QString)), SLOT(notify(QString)), Qt::QueuedConnection);

  foreach (const DownloadTaskInfo &t, Settings::globalSettings()->recentTasks())
    addTask(t);

  // - IPC -
  browserDelegate_ = new AcBrowser(this);
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

  //grabGesture(Qt::PanGesture);
  //grabGesture(Qt::SwipeGesture);
  //grabGesture(Qt::PinchGesture);
}

bool
MainWindow::isAddingUrls() const
{ return taskDialog_ && taskDialog_->isVisible(); }

void
MainWindow::createSearchEngines()
{
  enum { LastEngine = SearchEngineFactory::WikiZh };
  for (int i = 0; i <= LastEngine; i++) {
    SearchEngine *e = SearchEngineFactory::globalInstance()->create(i);
    e->setParent(this);
    searchEngines_.append(e);
  }
}

void
MainWindow::createLayout()
{
  AcUi *ui = AcUi::globalInstance();
  ui->setWindowStyle(this);
  ui->setStatusBarStyle(statusBar());

  startButton_ = ui->makeToolButton(AcUi::PushHint, tr("Start"), this, SLOT(start()));
  stopButton_ = ui->makeToolButton(AcUi::PushHint, tr("Stop"), this, SLOT(stop()));
  removeButton_ = ui->makeToolButton(AcUi::PushHint, tr("Remove"), this, SLOT(remove()));
  playButton_ = ui->makeToolButton(AcUi::PushHint, tr("Play"), this, SLOT(play()));
  browseButton_ = ui->makeToolButton(AcUi::PushHint, tr("Browse"), this, SLOT(browse()));
  openDirectoryButton_ = ui->makeToolButton(
        AcUi::PushHint, tr("Dir"), tr("Open directory"), this, SLOT(openDirectory()));
  addButton_ = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, tr("Add"), "", K_CTRL "+N", this, SLOT(add()));

  startButton_->setEnabled(false);
  stopButton_->setEnabled(false);
  removeButton_->setEnabled(false);
  playButton_->setEnabled(false);
  browseButton_->setEnabled(false);

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

    footer->addWidget(playButton_);
    footer->addWidget(browseButton_);
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
  setCentralWidget(new LayoutWidget(rows));

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
    startAllAct_->setText(tr("Start All"));
    startAllAct_->setStatusTip(tr("Start All"));
  } connect(startAllAct_, SIGNAL(triggered()), SLOT(startAll()));
  stopAllAct_ = new QAction(this); {
    stopAllAct_->setText(tr("Stop all"));
    stopAllAct_->setStatusTip(tr("Stop All"));
  } connect(stopAllAct_, SIGNAL(triggered()), SLOT(stopAll()));
  removeAllAct_ = new QAction(this); {
    removeAllAct_->setText(tr("Remove All"));
    removeAllAct_->setStatusTip(tr("Remove All"));
  } connect(removeAllAct_, SIGNAL(triggered()), SLOT(removeAll()));
  playAct_ = new QAction(this); {
    playAct_->setText(tr("Play"));
    playAct_->setStatusTip(tr("Open with Annot Player"));
  } connect(playAct_, SIGNAL(triggered()), SLOT(play()));
  browseAct_ = new QAction(this); {
    browseAct_->setText(tr("Browse"));
    browseAct_->setStatusTip(tr("Open with Annot Browser"));
  } connect(browseAct_, SIGNAL(triggered()), SLOT(browse()));
  openDirectoryAct_ = new QAction(this); {
    openDirectoryAct_->setText(tr("Open Directory"));
    openDirectoryAct_->setStatusTip(tr("Open Directory"));
  } connect(openDirectoryAct_, SIGNAL(triggered()), SLOT(openDirectory()));
  hideAct_ = new QAction(this); {
    hideAct_->setText(tr("Hide"));
    hideAct_->setStatusTip(tr("Hide"));
  } connect(hideAct_, SIGNAL(triggered()), SLOT(fadeOut()));
  quitAct_ = new QAction(this); {
    quitAct_->setText(tr("Quit"));
    quitAct_->setStatusTip(tr("Quit"));
    quitAct_->setShortcut(QKeySequence::Quit);
  } connect(quitAct_, SIGNAL(triggered()), SLOT(quit()));
  newAct_ = new QAction(this); {
    newAct_->setText(tr("New"));
    newAct_->setStatusTip(tr("New"));
    newAct_->setShortcut(QKeySequence::New);
  } connect(newAct_, SIGNAL(triggered()), SLOT(add()));
  menuBarAct_ = new QAction(this); {
    menuBarAct_->setCheckable(true);
    menuBarAct_->setText(tr("Menu Bar"));
    menuBarAct_->setStatusTip(tr("Menu Bar"));
  } connect(menuBarAct_, SIGNAL(triggered(bool)), menuBar(), SLOT(setVisible(bool)));
 copyTitleAct_ = new QAction(this); {
   copyTitleAct_->setStatusTip(tr("Copy Title"));
 } connect(copyTitleAct_, SIGNAL(triggered()), SLOT(copyTitle()));
 copyUrlAct_ = new QAction(this); {
   copyUrlAct_->setStatusTip(tr("Copy Url"));
 } connect(copyUrlAct_, SIGNAL(triggered()), SLOT(copyUrl()));

  // Copy menu
  copyMenu_ = new QMenu(tr("Copy") + " ...", this); {
    AcUi::globalInstance()->setContextMenuStyle(copyMenu_, true); // persistent = true
    copyMenu_->addAction(copyTitleAct_);
    copyMenu_->addAction(copyUrlAct_);

    copyMenu_->addSeparator();

    QString t = tr("Search with %1");

    SearchEngine *e = searchEngines_[SearchEngineFactory::Google];
    copyMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithGoogle()));

    e = searchEngines_[SearchEngineFactory::GoogleImages];
    copyMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithGoogleImages()));

    e = searchEngines_[SearchEngineFactory::Bing];
    copyMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithBing()));

    copyMenu_->addSeparator();

    e = searchEngines_[SearchEngineFactory::Nicovideo];
    copyMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithNicovideo()));

    e = searchEngines_[SearchEngineFactory::Bilibili];
    copyMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithBilibili()));

    e = searchEngines_[SearchEngineFactory::Acfun];
    copyMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithAcfun()));

    e = searchEngines_[SearchEngineFactory::Youtube];
    copyMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithYoutube()));

    e = searchEngines_[SearchEngineFactory::Youku];
    copyMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithYouku()));

    copyMenu_->addSeparator();

    e = searchEngines_[SearchEngineFactory::WikiEn];
    copyMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithWikiEn()));

    e = searchEngines_[SearchEngineFactory::WikiJa];
    copyMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithWikiJa()));

    e = searchEngines_[SearchEngineFactory::WikiZh];
    copyMenu_->addAction(QIcon(e->icon()), t.arg(e->name()), this, SLOT(searchCurrentTitleWithWikiZh()));

  } copyMenu_->setEnabled(false);

  // Create menus
  contextMenu_ = new QMenu(this); {
    AcUi::globalInstance()->setContextMenuStyle(contextMenu_, true); // persistent = true

    contextMenu_->addMenu(copyMenu_);
    contextMenu_->addSeparator();
    contextMenu_->addAction(startAct_);
    contextMenu_->addAction(playAct_);
    contextMenu_->addAction(browseAct_);
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
    contextMenu_->addAction(menuBarAct_);
    contextMenu_->addAction(tr("Preferences"), this, SLOT(preferences()), QKeySequence("CTRL+,"));
    contextMenu_->addAction(hideAct_);
    contextMenu_->addAction(quitAct_);
  #endif // Q_WS_MAC
    }

  hideAct_->setShortcut(QKeySequence("Esc"));
  new QShortcut(QKeySequence("Esc"), this, SLOT(fadeOut()));

  //quitAct_->setShortcut(QKeySequence("CTRL+W"));
  new QShortcut(QKeySequence("CTRL+W"), this, SLOT(fadeOut()));

  new QShortcut(QKeySequence::New, this, SLOT(add()));

  // Create menu bar

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
    m->addAction(playAct_);
    m->addAction(browseAct_);
    m->addAction(openDirectoryAct_);
#ifndef Q_WS_MAC
    m->addSeparator();
    m->addAction(tr("Quit"), this, SLOT(quit()), QKeySequence::Quit);
#endif // Q_WS_MAC
  }
  m = menuBar()->addMenu(tr("&Edit")); {
    m->addAction(startAct_);
    m->addSeparator();
    m->addAction(stopAct_);
    m->addAction(restartAct_);
    m->addAction(removeAct_);
  }
  m = menuBar()->addMenu(tr("&Settings")); {
    m->addAction(tr("&Preferences"), this, SLOT(preferences()), QKeySequence("CTRL+,"));
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

QString
MainWindow::currentTitle() const
{ DownloadTask *t = currentTask(); return t ? t->title() : QString(); }

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
  if (!downloadManager_->taskWithUrl(text)) {
    TaskDialog *dlg = taskDialog();
    if (dlg->isVisible())
      dlg->addText(text);
    else {
      dlg->setText(text);
      dlg->show();
    }
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
MainWindow::play()
{
  DownloadTask *t = currentTask();
  if (t) {
    QString url = QFile::exists(t->fileName()) ? t->fileName() : t->url();
    if (!url.isEmpty())
      playerDelegate_->openUrl(url);
  }
}

void
MainWindow::browse()
{
  DownloadTask *t = currentTask();
  if (t && !t->url().isEmpty())
    browserDelegate_->openUrl(t->url());
}

void
MainWindow::openDirectory()
{ AcLocationManager::globalInstance()->openDownloadsLocation(); }

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
  DOUT("enter");
  Q_ASSERT(task);
  if (!task->isFinished()) {
    Q_ASSERT(0);
    DOUT("exit: task is not finished");
    return;
  }
  QString file = task->fileName();

  QString title;
  if (QFile::exists(file)) {
    title = tr("download finished");
    QString msg = title + ": " + file;
    showMessage(msg);
    //emit downloadFinished(file, task->url());

    signer_->signFileWithUrl(file, task->url()); // async = false
  } else {
    title = tr("download failed");
    QString msg = title + ": " + file;
    warn(msg);
  }
  QApplication::beep();
  if (trayIcon_)
    trayIcon_->showMessage(title, file);
  AcLocationManager::globalInstance()->createDownloadsLocation();
  DOUT("exit");
}

void
MainWindow::updateButtons()
{
  DownloadTask *t = currentTask();
  if (t) {
    bool e = QFile::exists(t->fileName());
    startButton_->setEnabled(!t->isRunning() && !e);
    stopButton_->setEnabled(!t->isStopped());
    removeButton_->setEnabled(true);
    playButton_->setEnabled(true);
    browseButton_->setEnabled(true);
  } else {
    startButton_->setEnabled(false);
    stopButton_->setEnabled(false);
    removeButton_->setEnabled(false);
    playButton_->setEnabled(false);
    browseButton_->setEnabled(false);
  }
}

void
MainWindow::copyTitle()
{
  DownloadTask *t = currentTask();
  if (t && QApplication::clipboard()) {
    QString text = t->title();
    if (!text.isEmpty()) {
      QApplication::clipboard()->setText(text);
      showMessage(text);
    }
  }
}

void
MainWindow::copyUrl()
{
  DownloadTask *t = currentTask();
  if (t && QApplication::clipboard()) {
    QString text = t->url();
    if (!text.isEmpty()) {
      QApplication::clipboard()->setText(text);
      showMessage(text);
    }
  }
}

void
MainWindow::updateCopyMenu()
{
  copyUrlAct_->setText(tr("Copy URL"));
  copyTitleAct_->setText(tr("Copy Title"));

  DownloadTask *t = currentTask();
  if (t) {
    if (!t->url().isEmpty())
      copyUrlAct_->setText(t->url());
    if (!t->title().isEmpty())
      copyTitleAct_->setText(t->title());
  }
}

void
MainWindow::updateActions()
{
  menuBarAct_->setChecked(menuBar()->isVisible());

  DownloadTask *t = currentTask();
  if (t) {
    bool e = QFile::exists(t->fileName());
    copyMenu_->setEnabled(true);
    startAct_->setEnabled(!t->isRunning() && !e);
    restartAct_->setEnabled(true);
    stopAct_->setEnabled(!t->isStopped());
    removeAct_->setEnabled(true);
    playAct_->setEnabled(true);
    browseAct_->setEnabled(true);
  } else {
    copyMenu_->setEnabled(false);
    startAct_->setEnabled(false);
    restartAct_->setEnabled(false);
    stopAct_->setEnabled(false);
    removeAct_->setEnabled(false);
    playAct_->setEnabled(false);
    browseAct_->setEnabled(false);
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
        url = QUrl::fromPercentEncoding(url.toLocal8Bit());
      if (!url.isEmpty())
        QTimer::singleShot(0, new slot_::PromptUrl(url, this), SLOT(trigger()));
    } break;
  case QEvent::Gesture:
    DOUT("gesture event");
  default: accept = Base::event(e);
  }
  return accept;
}

void
MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
  Q_ASSERT(event);
  updateActions();
  if (copyMenu_->isEnabled())
    updateCopyMenu();
  contextMenu_->exec(event->globalPos());
  event->accept();
}

void
MainWindow::keyPressEvent(QKeyEvent *e)
{
  switch (e->key()) {
  case Qt::Key_CapsLock:
#ifdef Q_WS_WIN
    menuBar()->setVisible(!menuBar()->isVisible());
#endif // Q_WS_WIN
    break;
  default: ;
  }
  Base::keyReleaseEvent(e);
}

void
MainWindow::about()
{
  if (!about_)
    about_ = new AcAbout(G_APPLICATION, G_VERSION, this);
  about_->show();
}

void
MainWindow::preferences()
{
  if (!preferences_)
    preferences_ = new AcPreferences(this);
  preferences_->show();
}

void
MainWindow::quit()
{
  fadeOut();
  QTimer::singleShot(fadeAnimation()->duration(), this, SLOT(close()));
}

void
MainWindow::searchWithEngine(int engine, const QString &key)
{
  if (engine >= 0 && engine < searchEngines_.size() && !key.isEmpty()) {
    const SearchEngine *e = searchEngines_[engine];
    if (e) {
      QString url = e->search(key);
      if (!url.isEmpty()) {
        showMessage(url);
        browserDelegate_->openUrl(url);
      }
    }
  }
}

// EOF
