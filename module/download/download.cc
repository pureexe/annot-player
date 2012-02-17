// download.cc
// 2/4/2012

#include "download.h"
#include "downloader.h"
#include <QtCore>
#include <QtNetwork>

#define DEBUG "download"
#include "module/debug/debug.h"

bool
dlget(const QString &path, const QUrl &url, const QString &header,
      bool zipped, bool async, int retries)
{
  DOUT("enter: path =" << path << ", url =" << url << ", zipped =" << zipped << ", async =" << async << ", retries =" << retries);
  Downloader dl(path, zipped);
  dl.get(url, header, async, retries);
  bool ret = dl.state() != Downloader::Error;
  DOUT("exit: ret =" << ret << ", state =" << dl.state());
  return ret;
}

bool
dlpost(const QString &path, const QUrl &url,
       const QByteArray &data, const QString &header,
       bool zipped, bool async,
       int retries)
{
  DOUT("enter: path =" << path << ", url =" << url << ", header =" << header << ", zipped =" << zipped << ", async =" << async << ", retries =" << retries);
  Downloader dl(path, zipped);
  dl.post(url, data, header, async, retries);
  bool ret = dl.state() != Downloader::Error;
  DOUT("exit: ret =" << ret << ", state =" << dl.state());
  return ret;
}

// EOF
