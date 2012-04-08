// mainwindow.cc
// 2/17/2012

#include "mainwindow.h"
#include "mainwindowprivate.h"
#include "taskdialog.h"
#include "clipboardmonitor.h"
#include "signer.h"
#include "rc.h"
#include "ac/acui.h"
#include "ac/acss.h"
#include "ac/acfilteredlistview.h"
#include "module/qtext/datetime.h"
#include "module/download/downloadmanager.h"
#include "module/download/mrldownloadtask.h"
#include <QtGui>
#include <climits>

#define DEBUG "mainwindow"
#include "module/debug/debug.h"

#ifdef Q_OS_MAC
  #define K_CTRL        "cmd"
#else
  #define K_CTRL        "Ctrl"
#endif // Q_OS_MAC

enum { RefreshInterval = 3000 };

// - Constructions -

//#define WINDOW_FLAGS (
//  Qt::CustomizeWindowHint |
//  Qt::WindowTitleHint |
//  Qt::WindowSystemMenuHint |
//  Qt::WindowMinMaxButtonsHint |
//  Qt::WindowCloseButtonHint )

MainWindow::MainWindow(QWidget *parent)
  : Base(parent), taskDialog_(0)
{
  setWindowTitle(tr("Annot Downloader"));
  setWindowIcon(QIcon(RC_IMAGE_APP));

  downloadManager_ = new DownloadManager(this);

  createModels();
  createLayout();
  createActions();

  refreshTimer_ = new QTimer(this);
  refreshTimer_->setInterval(RefreshInterval);
  connect(refreshTimer_, SIGNAL(timeout()), SLOT(refresh()));

  connect(tableView_, SIGNAL(currentIndexChanged(QModelIndex)), SLOT(invalidateButtons()));

  clipboardMonitor_ = new ClipboardMonitor(this);
  connect(clipboardMonitor_, SIGNAL(message(QString)), SLOT(showMessage(QString)));
  connect(clipboardMonitor_, SIGNAL(warning(QString)), SLOT(warn(QString)));
  connect(clipboardMonitor_, SIGNAL(error(QString)), SLOT(error(QString)));
  connect(clipboardMonitor_, SIGNAL(annotationUrlEntered(QString)), SLOT(promptUrl(QString)));
  connect(clipboardMonitor_, SIGNAL(mediaUrlEntered(QString)), SLOT(promptUrl(QString)));

  signer_ = new Signer(this);
  connect(signer_, SIGNAL(message(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(signer_, SIGNAL(warning(QString)), SLOT(warn(QString)), Qt::QueuedConnection);
  connect(signer_, SIGNAL(error(QString)), SLOT(error(QString)), Qt::QueuedConnection);
  connect(this, SIGNAL(downloadFinished(QString,QString)), signer_, SLOT(signFileWithUrl(QString,QString)));

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
  //openButton_ = ui->makeToolButton(AcUi::PushHint, TR(T_PLAY), this, SLOT(open()));
  openDirectoryButton_ = ui->makeToolButton(
        AcUi::PushHint, tr("Dir"), tr("Open directory"), this, SLOT(openDirectory()));
  addButton_ = ui->makeToolButton(
        AcUi::PushHint | AcUi::HighlightHint, tr("Add"), "", K_CTRL "+N", this, SLOT(add()));

  startButton_->setEnabled(false);
  stopButton_->setEnabled(false);
  removeButton_->setEnabled(false);
  //openButton_->setEnabled(false);

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

    //footer->addWidget(openButton_);
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
  //QShortcut *c1 = new QShortcut(QKeySequence("CTRL+1"), this);
  //connect(c1, SIGNAL(activated()), textTabButton_, SLOT(click()));
  //QShortcut *c2 = new QShortcut(QKeySequence("CTRL+2"), this);
  //connect(c2, SIGNAL(activated()), userTabButton_, SLOT(click()));

  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));

  QShortcut *closeShortcut = new QShortcut(QKeySequence::Close, this);
  connect(closeShortcut, SIGNAL(activated()), SLOT(hide()));

  QShortcut *newShortcut = new QShortcut(QKeySequence::New, this);
  connect(newShortcut, SIGNAL(activated()), SLOT(add()));
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

void
MainWindow::stopAll()
{ downloadManager_->stopAll(); }

TaskDialog*
MainWindow::taskDialog()
{
  if (!taskDialog_) {
    taskDialog_ = new TaskDialog(this);
    connect(taskDialog_, SIGNAL(urlsAdded(QStringList,bool)), SLOT(addUrls(QStringList,bool)));
    connect(taskDialog_, SIGNAL(message(QString)), SLOT(showMessage(QString)));
    connect(taskDialog_, SIGNAL(warning(QString)), SLOT(warn(QString)));
    connect(taskDialog_, SIGNAL(error(QString)), SLOT(error(QString)));
  }
  return taskDialog_;
}

void
MainWindow::promptUrls(const QStringList &urls)
{
  foreach (QString text, urls)
    promptUrl(text);
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
  if (t && !t->isRunning())
    t->run();
}

void
MainWindow::stop()
{
  DownloadTask *t = currentTask();
  if (t && t->isRunning())
    t->stop();
}

void
MainWindow::remove()
{
  DownloadTask *t = currentTask();
  if (t) {
    if (t->isRunning())
      t->stop();
  }
  tableView_->removeCurrentRow();
}

void
MainWindow::open()
{
  DownloadTask *t = currentTask();
  if (t && !t->fileName().isEmpty())
    emit openFileRequested(t->fileName());
}

void
MainWindow::openDirectory()
{
  QString desktopPath = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  QString url =
#ifdef Q_OS_WIN
    "file:///" + desktopPath
#else
    "file://" + desktopPath
#endif // Q_OS_WIN
  ;
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
  //log(tr("analyzing URL") + ": " + url);

  // TODO: add support to demux AVI/MP3 for nico flv
  DownloadTask *t = new MrlDownloadTask(url, downloadManager_);

  t->setAutoDelete(false);
  connect(t, SIGNAL(error(QString)), SLOT(showMessage(QString)), Qt::QueuedConnection);
  connect(t, SIGNAL(finished(DownloadTask*)), SLOT(finish(DownloadTask*)), Qt::QueuedConnection);
  QThreadPool::globalInstance()->start(t);

  //DOUT("eventloop enter");
  //QEventLoop loop;
  //connect(t, SIGNAL(error(QString)), &loop, SLOT(quit()));
  //connect(t, SIGNAL(finished()), &loop, SLOT(quit()));
  //connect(t, SIGNAL(fileNameChanged(QString)), &loop, SLOT(quit()));
  //loop.exec();
  //DOUT("eventloop leave");

  addTask(t);
  DOUT("exit");
}

void
MainWindow::addTask(DownloadTask *t)
{
  Q_ASSERT(t);
  downloadManager_->addTask(t);

#define FORMAT_TIME(_msecs)       downloadTimeToString(_msecs)
#define FORMAT_STATE(_state)      downloadStateToString(_state)
#define FORMAT_PERCENTAGE(_real)  QString().sprintf("%.2f%%", (_real)*100)
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

  tableView_->invalidateCount();
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
#define FORMAT_PERCENTAGE(_real)  QString().sprintf("%.2f%%", (_real)*100)
#define FORMAT_SIZE(_size)        downloadSizeToString(_size)
#define FORMAT_SPEED(_speed)      downloadSpeedToString(_speed)

  qreal totalSpeed = 0;
  qreal leastPercentage = 0;
  qint64 leastRemainingTime = LLONG_MAX;

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
      leastRemainingTime = qMin(leastRemainingTime, t->remainingTime());
      leastPercentage = qMin(leastPercentage, t->percentage());
    }

    if (t->isRunning())
      totalSpeed += t->speed();
  }

  tableView_->invalidateCount();
  invalidateButtons();

  QString title;
  if (totalSpeed > 0) {
    title += " (";
    title += FORMAT_SPEED(totalSpeed);
    if (leastPercentage)
      title += ", " + FORMAT_PERCENTAGE(leastPercentage);
    if (leastRemainingTime) {
      QString ts = FORMAT_TIME(leastRemainingTime);
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
  if (QFile::exists(file)) {
    showMessage(tr("download finished") + ": " + file);
    emit downloadFinished(file, task->url());
  } else
    warn(tr("download failed") + ": " + file);
  QApplication::beep();
}

void
MainWindow::invalidateButtons()
{
  DownloadTask *t = currentTask();
  if (!t) {
    startButton_->setEnabled(false);
    stopButton_->setEnabled(false);
    removeButton_->setEnabled(false);
    //openButton_->setEnabled(false);
  } else {
    bool e = QFile::exists(t->fileName());
    startButton_->setEnabled(!t->isRunning() && !e);
    stopButton_->setEnabled(t->isRunning());
    removeButton_->setEnabled(true);
    //openButton_->setEnabled(t->isFinished());
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
    return QString().sprintf("%.1f MB", size / (1024.0 * 1024));
}

QString
MainWindow::downloadSpeedToString(qreal speed) const
{
  if (speed < 1024)
    return QString::number((int)speed) + " B/s";
  else if (speed < 1024 * 1024)
    return QString::number((int)speed / 1024) + " KB/s";
  else
    return QString().sprintf("%.2f MB/s", speed / (1024.0 * 1024));
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
  hide();

  stopAll();

  signer_->dispose();

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
      QFileOpenEvent *fe = dynamic_cast<QFileOpenEvent *>(e);
      Q_ASSERT(fe);
      if (fe) {
        QString url = fe->file();
        QTimer::singleShot(0, new slot_::PromptUrl(url, this), SLOT(promptUrl()));
      }
    } break;
  default: accept = Base::event(e);
  }
  return accept;
}

// EOF
