#ifndef MRLMERGEDOWNLOADTASK_H
#define MRLMERGEDOWNLOADTASK_H

// mrlmergedownloadtask.h
// 3/16/2012

#include "downloadtask.h"
#include "module/mrlresolver/mrlinfo.h"

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

class MrlResolver;
class RemoteStream;
class FileOutputStream;
class BufferedStreamPipe;

class MrlMergeDownloadTask : public DownloadTask
{
  Q_OBJECT
  typedef MrlMergeDownloadTask Self;
  typedef DownloadTask Base;

  RemoteStream *in_;
  FileOutputStream *out_;
  BufferedStreamPipe *pipe_;
  MrlResolver *resolver_;
  bool stopped_;

public:
  explicit MrlMergeDownloadTask(const QString &url, QObject *parent = 0);
  ~MrlMergeDownloadTask() { stop(); }

  // - Actions -
signals:
  void stopped();
public slots:
  virtual void run(); ///< \override
  virtual void stop(); ///< \override

  virtual void finish();  ///< \override

protected slots:
  void downloadMedia(const MediaInfo &mi, QNetworkCookieJar *jar = 0);

protected:
  static QString escapeFileName(const QString &name);
};

#endif // MRLMERGEDOWNLOADTASK_H
