#ifndef MRLDOWNLOADTASK_H
#define MRLDOWNLOADTASK_H

// mrldownloadtask.h
// 2/20/2012

#include "module/downloadtask/downloadtask.h"
#include "module/mrlresolver/mrlinfo.h"
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

public:
  explicit MrlDownloadTask(const QString &url, QObject *parent = 0)
    : Base(url, parent), stopped_(false), resolver_(0), retries_(5) { }
  explicit MrlDownloadTask(const DownloadTaskInfo &info, QObject *parent = 0)
    : Base(info, parent), stopped_(false), resolver_(0), retries_(5) {  }

  ~MrlDownloadTask();

  // - Actions -
signals:
  void stopped();
public slots:
  virtual void run(); ///< \override
  virtual void stop(); ///< \override
  virtual void reset(); ///< \override

protected slots:
  void downloadMedia(const MediaInfo &mi, QNetworkCookieJar *jar);

  void downloadSingleMedia(const MediaInfo &mi, QNetworkCookieJar *jar);
  void downloadMultipleMedia(const MediaInfo &mi, QNetworkCookieJar *jar);

  void updateProgressWithId(qint64 receivedBytes, qint64 totalBytes, long id);

protected:
  static bool isMultiMediaMimeType(const QString &contentType);
};

#endif // MRLDOWNLOADTASK_H
