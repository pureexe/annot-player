#ifndef MRLMUXDOWNLOADTASK_H
#define MRLMUXDOWNLOADTASK_H

// mrlmuxdownloadtask.h
// 2/22/2012

#include "downloadtask.h"
#include "module/mrlresolver/mrlinfo.h"
#include <QtCore/QHash>
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

class MrlResolver;
class RemoteStream;
class FileOutputStream;
class FlvListDemux;

class MrlMuxDownloadTask : public DownloadTask
{
  Q_OBJECT
  typedef MrlMuxDownloadTask Self;
  typedef DownloadTask Base;

  MrlResolver *resolver_;
  QList<RemoteStream*> ins_;
  FileOutputStream *videoOut_, *audioOut_;
  FlvListDemux *demux_;
  bool stopped_;

  struct Progress { qint64 receivedBytes, totalBytes; };
  QHash<long, Progress> progresses_;

public:
  explicit MrlMuxDownloadTask(const QString &url, QObject *parent = 0);
  ~MrlMuxDownloadTask() { stop(); }

  // - Actions -
signals:
  void stopped();
public slots:
  virtual void run(); ///< \override
  virtual void stop(); ///< \override

  virtual void finish();  ///< \override

protected slots:
  void downloadAndMuxMedia(const MediaInfo &mi, QNetworkCookieJar *jar = 0);
  void updateProgressWithId(qint64 receivedBytes, qint64 totalBytes, long id); ///< \internal

private:
  void downloadMedia(const QString &path);

protected:
  bool mux();
  static QString escapeFileName(const QString &name);
  static QString suffixForUrl(const QString &url, const QString &codecs = QString());
};

#endif // MRLMUXDOWNLOADTASK_H
