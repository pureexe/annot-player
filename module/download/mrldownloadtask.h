#ifndef MRLDOWNLOADTASK_H
#define MRLDOWNLOADTASK_H

// mrldownloadtask.h
// 2/20/2012

#include "downloadtask.h"
#include "module/mrlresolver/mrlinfo.h"
#include <QHash>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

class MrlResolver;
class MrlDownloadTask : public DownloadTask
{
  Q_OBJECT
  typedef MrlDownloadTask Self;
  typedef DownloadTask Base;

  MrlResolver *resolver_;
  bool stopped_;

  struct Progress
  {
    qint64 receivedBytes, totalBytes;
    explicit Progress(qint64 r = 0, qint64 t = 0) : receivedBytes(r), totalBytes(t) { }
  };
  QHash<long, Progress> progress_;

public:
  explicit MrlDownloadTask(const QString &mrl, QObject *parent = 0);
  ~MrlDownloadTask() { stop(); }

  // - Actions -
signals:
  void stopped();
public slots:
  virtual void run(); ///< \override
  virtual void stop(); ///< \override

protected slots:
  void downloadMedia(const MediaInfo &mi, QNetworkCookieJar *jar = 0);

  void downloadSingleMedia(const MediaInfo &mi, QNetworkCookieJar *jar = 0);
  void downloadMultipleMedia(const MediaInfo &mi, QNetworkCookieJar *jar = 0);

  void updateProgressWithId(qint64 receivedBytes, qint64 totalBytes, long id);

protected:
  static bool isMultiMediaMimeType(const QString &contentType);
};

#endif // MRLDOWNLOADTASK_H
