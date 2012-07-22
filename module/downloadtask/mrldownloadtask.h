#ifndef MRLDOWNLOADTASK_H
#define MRLDOWNLOADTASK_H

// mrldownloadtask.h
// 2/20/2012

#include "module/downloadtask/downloadtask.h"
#include "module/mrlresolver/mrlinfo.h"
#include "module/qtext/os.h"
#include <QtCore/QHash>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

class MrlResolver;
class MrlDownloadTask : public DownloadTask
{
  Q_OBJECT
  Q_DISABLE_COPY(MrlDownloadTask)
  typedef MrlDownloadTask Self;
  typedef DownloadTask Base;

  bool stopped_;
  MrlResolver *resolver_;

  struct Progress
  {
    qint64 receivedBytes, totalBytes;
    explicit Progress(qint64 r = 0, qint64 t = 0) : receivedBytes(r), totalBytes(t) { }
  };
  QHash<long, Progress> progress_;

  int retries_;

  QtExt::SleepTimer sleep_;

public:
  explicit MrlDownloadTask(const QString &url, QObject *parent = nullptr)
    : Base(url, parent), stopped_(false), resolver_(0), retries_(5) { }
  explicit MrlDownloadTask(const DownloadTaskInfo &info, QObject *parent = nullptr)
    : Base(info, parent), stopped_(false), resolver_(0), retries_(5) {  }

  ~MrlDownloadTask();

signals:
  void stopped();
  // - Actions -
public slots:
  virtual void run(bool exec);

  void stop() override;
  void reset() override;

protected slots:
  void downloadMedia(const MediaInfo &mi, QNetworkCookieJar *jar);

  void downloadSingleMedia(const MediaInfo &mi, QNetworkCookieJar *jar);
  void downloadMultipleMedia(const MediaInfo &mi, QNetworkCookieJar *jar);

  void updateProgressWithId(qint64 receivedBytes, qint64 totalBytes, long id);

protected:
  static bool isMultiMediaMimeType(const QString &contentType);
};

#endif // MRLDOWNLOADTASK_H
