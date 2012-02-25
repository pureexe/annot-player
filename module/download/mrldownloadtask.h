#ifndef MRLDOWNLOADTASK_H
#define MRLDOWNLOADTASK_H

// mrldownloadtask.h
// 2/20/2012

#include "downloadtask.h"
#include "module/mrlresolver/mrlinfo.h"

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

class MrlResolver;
class RemoteStream;
class FileOutputStream;
class BufferedStreamPipe;

class MrlDownloadTask : public DownloadTask
{
  Q_OBJECT
  typedef MrlDownloadTask Self;
  typedef DownloadTask Base;

  RemoteStream *in_;
  FileOutputStream *out_;
  BufferedStreamPipe *pipe_;
  MrlResolver *resolver_;
  bool stopped_;

public:
  explicit MrlDownloadTask(const QString &url, QObject *parent = 0);
  ~MrlDownloadTask() { stop(); }

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

#endif // MRLDOWNLOADTASK_H
