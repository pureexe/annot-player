// downloaddialog.cc
// 2/17/2012

#include "downloaddialog.h"
#include "downloadtaskdialog.h"
#include "filteredlistview.h"
#include "uistyle.h"
#include "tr.h"
#include "defines.h"
#include "stylesheet.h"
#include "logger.h"
#include "module/qtext/toolbutton.h"
#include "module/qtext/datetime.h"
#include "module/download/downloadmanager.h"
#include "module/download/mrldownloadtask.h"
#include "module/download/mrlmuxdownloadtask.h"
#include <QtGui>
#include <climits>

#define DEBUG "downloaddialog"
#include "module/debug/debug.h"

using namespace Logger;

#ifdef Q_OS_MAC
  #define K_CTRL        "cmd"
#else
  #define K_CTRL        "Ctrl"
#endif // Q_OS_MAC

#define REFRESH_INTERVAL        3000 // 3 second

// - Constructions -

#define WINDOW_FLAGS ( \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint )

DownloadDialog::DownloadDialog(QWidget *parent)
  : Base(parent, WINDOW_FLAGS), downloadTaskDialog_(0)
{
  setWindowTitle(TR(T_TITLE_DOWNLOAD));
  UiStyle::globalInstance()->setWindowStyle(this);

  downloadManager_ = new DownloadManager(this);

  createModels();
  createLayout();
  createActions();

  refreshTimer_ = new QTimer(this);
  refreshTimer_->setInterval(REFRESH_INTERVAL);
  connect(refreshTimer_, SIGNAL(timeout()), SLOT(refresh()));

  connect(tableView_, SIGNAL(currentIndexChanged(QModelIndex)), SLOT(invalidateButtons()));

  tableView_->setFocus();
}

void
DownloadDialog::createLayout()
{
#define MAKE_BUTTON(_button, _title, _tip, _slot) \
    _button = new QtExt::ToolButton; { \
      _button->setStyleSheet(SS_TOOLBUTTON_TEXT); \
      _button->setToolButtonStyle(Qt::ToolButtonTextOnly); \
      _button->setText(QString("[ %1 ]").arg(_title)); \
      _button->setToolTip(_tip); \
      connect(_button, SIGNAL(clicked()), _slot); \
    }

  MAKE_BUTTON(startButton_, TR(T_START), TR(T_START), SLOT(start()));
  MAKE_BUTTON(stopButton_, tr("Stop"), tr("Stop"), SLOT(stop()));
  MAKE_BUTTON(removeButton_, tr("Remove"), tr("Remove"), SLOT(remove()));
  MAKE_BUTTON(openButton_, TR(T_PLAY), TR(T_PLAY), SLOT(open()));
  MAKE_BUTTON(openDirectoryButton_, tr("Dir"), tr("Open directory"), SLOT(openDirectory()));
  MAKE_BUTTON(addButton_, TR(T_ADD), TR(T_ADD), SLOT(add()));

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
    if (!UiStyle::isAeroAvailable())
      patch = 9;

    // left, top, right, bottom
    header->setContentsMargins(0, 0, 0, 0);
    footer->setContentsMargins(0, 0, 0, 0);
    rows->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(9, patch, 9, patch);

  } setLayout(rows);

#undef MAKE_BUTTON
}

void
DownloadDialog::createModels()
{
  sourceModel_ = new QStandardItemModel(0, HD_Count, this);
  setHeaderData(sourceModel_);

  proxyModel_ = new QSortFilterProxyModel; {
    proxyModel_->setSourceModel(sourceModel_);
    proxyModel_->setDynamicSortFilter(true);
    proxyModel_->setSortCaseSensitivity(Qt::CaseInsensitive);
  }

  tableView_ = new FilteredListView(sourceModel_, proxyModel_, this);
}

void
DownloadDialog::createActions()
{
  //QShortcut *c1 = new QShortcut(QKeySequence("CTRL+1"), this);
  //connect(c1, SIGNAL(activated()), textTabButton_, SLOT(click()));
  //QShortcut *c2 = new QShortcut(QKeySequence("CTRL+2"), this);
  //connect(c2, SIGNAL(activated()), userTabButton_, SLOT(click()));

  QShortcut *cancelShortcut = new QShortcut(QKeySequence("Esc"), this);
  connect(cancelShortcut, SIGNAL(activated()), SLOT(hide()));
}

// - Table -

void
DownloadDialog::setHeaderData(QAbstractItemModel *model)
{
  Q_ASSERT(model);
  model->setHeaderData(HD_Name, Qt::Horizontal, TR(T_NAME));
  model->setHeaderData(HD_State, Qt::Horizontal, TR(T_STATE));
  model->setHeaderData(HD_Size, Qt::Horizontal, TR(T_SIZE));
  model->setHeaderData(HD_Speed, Qt::Horizontal, TR(T_SPEED));
  model->setHeaderData(HD_Percentage, Qt::Horizontal, "%");
  model->setHeaderData(HD_RemainingTime, Qt::Horizontal, tr("Remaining time"));
  model->setHeaderData(HD_Path, Qt::Horizontal, TR(T_PATH));
  model->setHeaderData(HD_Url, Qt::Horizontal, TR(T_URL));
  model->setHeaderData(HD_Id, Qt::Horizontal, TR(T_ID));
}

void
DownloadDialog::clear()
{
  tableView_->clear();
  setHeaderData(sourceModel_);
}

int
DownloadDialog::currentId() const
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
DownloadDialog::currentTask() const
{
  int id = currentId();
  if (id <= 0)
    return 0;
  return downloadManager_->taskWithId(id);
}

// - Actions -

void
DownloadDialog::stopAll()
{ downloadManager_->stopAll(); }

DownloadTaskDialog*
DownloadDialog::downloadTaskDialog()
{
  if (!downloadTaskDialog_) {
    downloadTaskDialog_ = new DownloadTaskDialog(this);
    connect(downloadTaskDialog_, SIGNAL(urlsAdded(QStringList,bool)), SLOT(addUrls(QStringList,bool)));
  }
  return downloadTaskDialog_;
}

void
DownloadDialog::promptDownloads(const QString urls)
{
  DownloadTaskDialog *dlg = downloadTaskDialog();
  if (dlg->isVisible())
    dlg->addText(urls);
  else {
    dlg->setText(urls);
    dlg->show();
  }
}

void
DownloadDialog::add()
{ downloadTaskDialog()->show(); }

void
DownloadDialog::start()
{
  DownloadTask *t = currentTask();
  if (t && !t->isRunning())
    t->run();
}

void
DownloadDialog::stop()
{
  DownloadTask *t = currentTask();
  if (t && t->isRunning())
    t->stop();
}

void
DownloadDialog::remove()
{
  DownloadTask *t = currentTask();
  if (t) {
    if (t->isRunning())
      t->stop();
  }
  tableView_->removeCurrentRow();
}

void
DownloadDialog::open()
{
  DownloadTask *t = currentTask();
  if (t && !t->path().isEmpty())
    emit openFileRequested(t->path());
}

void
DownloadDialog::openDirectory()
{
  QString desktopPath = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
  QDesktopServices::openUrl("file://" + desktopPath);
}

// - Download -

void
DownloadDialog::addUrls(const QStringList &urls, bool batch)
{
  Q_UNUSED(batch);
  foreach (QString url, urls)
    addUrl(url);
}

void
DownloadDialog::addUrl(const QString &url)
{
  DOUT("enter: url =" << url);
  log(tr("analyzing URL") + ": " + url);

  // TODO: add support to demux AVI/MP3 for nico flv
  DownloadTask *t = url.contains(".nicovideo.jp/", Qt::CaseInsensitive) ?
        (DownloadTask *)new MrlDownloadTask(url, downloadManager_) :
        (DownloadTask *)new MrlMuxDownloadTask(url, downloadManager_);

  t->setAutoDelete(false);
  connect(t, SIGNAL(error(QString)),
          LoggerSignals::globalInstance(), SLOT(error(QString)), Qt::QueuedConnection);
  connect(t, SIGNAL(finished(DownloadTask*)), SLOT(finish(DownloadTask*)), Qt::QueuedConnection);
  QThreadPool::globalInstance()->start(t);

  //DOUT("eventloop enter");
  //QEventLoop loop;
  //connect(t, SIGNAL(error(QString)), &loop, SLOT(quit()));
  //connect(t, SIGNAL(finished()), &loop, SLOT(quit()));
  //connect(t, SIGNAL(pathChanged(QString)), &loop, SLOT(quit()));
  //loop.exec();
  //DOUT("eventloop leave");

  addTask(t);
  DOUT("exit");
}

void
DownloadDialog::addTask(DownloadTask *t)
{
  Q_ASSERT(t);
  downloadManager_->addTask(t);

#define FORMAT_TIME(_msecs)       downloadTimeToString(_msecs)
#define FORMAT_STATE(_state)      downloadStateToString(_state)
#define FORMAT_PERCENTAGE(_real)  QString().sprintf("%.1f%%", (_real)*100)
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
DownloadDialog::refresh()
{
  if (downloadManager_->isEmpty()) {
    setWindowTitle(TR(T_DOWNLOAD));
    return;
  }

#define FORMAT_TIME(_msecs)       downloadTimeToString(_msecs)
#define FORMAT_STATE(_state)      downloadStateToString(_state)
#define FORMAT_PERCENTAGE(_real)  QString().sprintf("%.1f%%", (_real)*100)
#define FORMAT_SIZE(_size)        downloadSizeToString(_size)
#define FORMAT_SPEED(_speed)      downloadSpeedToString(_speed)

  qreal totalSpeed = 0;
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
    sourceModel_->setData(sourceModel_->index(row, HD_Path), t->path(), Qt::DisplayRole);
    sourceModel_->setData(sourceModel_->index(row, HD_Url), t->url(), Qt::DisplayRole);
    //sourceModel_->setData(sourceModel_->index(row, HD_Id), t->id(), Qt::DisplayRole);

    if (!t->isFinished())
      leastRemainingTime = qMin(leastRemainingTime, t->remainingTime());

    totalSpeed += t->speed();
  }

  tableView_->invalidateCount();
  invalidateButtons();

  QString title = totalSpeed <= 0 ? TR(T_DOWNLOAD) : leastRemainingTime == LLONG_MAX ?
    QString("%1 (%2)").arg(TR(T_DOWNLOAD)).arg(FORMAT_SPEED(totalSpeed)) :
    QString("%1 (%2, %3)").arg(TR(T_DOWNLOAD)).arg(FORMAT_SPEED(totalSpeed)).arg(FORMAT_TIME(leastRemainingTime)) ;
  setWindowTitle(title);
#undef FORMAT_TIME
#undef FORMAT_STATE
#undef FORMAT_PERCENTAGE
#undef FORMAT_SIZE
#undef FORMAT_SPEED
}

// - Events -

void
DownloadDialog::finish(DownloadTask *task)
{
  Q_ASSERT(task);
  if (!task->isFinished())
    return;
  QString path = task->path();
  if (QFile::exists(path)) {
    log(tr("download finished") + ": " + path);
    emit downloadFinished(path, task->url());
  } else
    warn(tr("download failed") + ": " + path);
  QApplication::beep();
}

void
DownloadDialog::invalidateButtons()
{
  DownloadTask *t = currentTask();
  if (!t) {
    startButton_->setEnabled(false);
    stopButton_->setEnabled(false);
    removeButton_->setEnabled(false);
    openButton_->setEnabled(false);
  } else {
    bool e = QFile::exists(t->path());
    startButton_->setEnabled(!t->isRunning() && !e);
    stopButton_->setEnabled(t->isRunning());
    removeButton_->setEnabled(true);
    openButton_->setEnabled(t->isFinished());
  }
}

void
DownloadDialog::setVisible(bool visible)
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
DownloadDialog::downloadStateToString(int state) const
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
DownloadDialog::downloadSizeToString(qint64 size) const
{
  if (size < 1024)
    return QString::number(size) + " B";
  else if (size < 1024 * 1024)
    return QString::number(size / 1014) + " KB";
  else
    return QString().sprintf("%.1f MB", size / (1024.0 * 1024));
}

QString
DownloadDialog::downloadSpeedToString(qreal speed) const
{
  if (speed < 1024)
    return QString::number((int)speed) + " B/s";
  else if (speed < 1024 * 1024)
    return QString::number((int)speed / 1024) + " KB/s";
  else
    return QString().sprintf("%.2f MB/s", speed / (1024.0 * 1024));
}

QString
DownloadDialog::downloadTimeToString(qint64 msecs) const
{
  QString ret = QtExt::msecs2time(msecs).toString();
  if (ret.isEmpty())
    ret = "-";
  return ret;
}

// EOF
