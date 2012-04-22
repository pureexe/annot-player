// downloadtask.cc
// 2/20/2012
#include "downloadtask.h"
#include <QtCore/QTimer>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

//#define DEBUG "downloadtask"
#include "module/debug/debug.h"

// - Construction -

int DownloadTask::count_ = 0;

#define INITIALIZER \
    id_(++count_), receivedSize_(0), totalSize_(0), progressUpdateTime_(0), speed_(0)

#define STATE_FROM_INT(_v) \
  ( \
    (_v) == Error ? Error : \
    (_v) == Stopped ? Stopped : \
    (_v) == Pending ? Pending : \
    (_v) == Downloading ? Pending : \
    (_v) == Finished ? Finished : \
    Stopped \
  )

DownloadTask::DownloadTask(const QString &url, QObject *parent)
  : Base(parent), INITIALIZER, state_(Stopped), url_(url)
{ init(); }

DownloadTask::DownloadTask(const DownloadTaskInfo &info, QObject *parent)
  : Base(parent), INITIALIZER,
    state_(STATE_FROM_INT(info.state)), url_(info.url), title_(info.title), fileName_(info.fileName)
{ init(); }

void
DownloadTask::init()
{
  connect(this, SIGNAL(progress(qint64,qint64)), SLOT(updateProgress(qint64,qint64)));
  if (downloadPath_.isEmpty())
    downloadPath_ = QDir::homePath();
}

void
DownloadTask::startLater(qint64 msecs)
{ QTimer::singleShot(msecs, this, SLOT(start())); }

// - Properties -

void
DownloadTask::setState(State s)
{
  //if (state_ != s) {
    state_ = s;
    emit stateChanged(state_);
  //}
}

void
DownloadTask::updateProgress(qint64 receivedBytes, qint64 totalBytes)
{
  enum { update_interval = 2500 }; // 2.5 seconds

  qint64 currentTime  = QDateTime::currentMSecsSinceEpoch();

  if (currentTime - progressUpdateTime_ < update_interval
      && !isFinished() && receivedBytes != totalBytes)
    return;

  totalSize_ = totalBytes;

  if (totalSize_ > 0 && receivedBytes > receivedSize_)
    speed_ = (receivedBytes - receivedSize_) * 1000.0 / (currentTime - progressUpdateTime_);
  else
    speed_ = 0;

  receivedSize_ = receivedBytes;
  progressUpdateTime_ = currentTime;
}

// EOF
