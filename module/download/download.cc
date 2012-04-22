// download.cc
// 2/4/2012

#include "download.h"
#include "downloader.h"
#include <QtNetwork/QNetworkRequest>

#define DEBUG "download"
#include "module/debug/debug.h"

bool
dlget(const QString &path, const QNetworkRequest &req,
      bool async, int retries, QObject *parent)
{
  DOUT("enter: path =" << path << ", url =" << req.url() << ", async =" << async << ", retries =" << retries);
  bool ret = false;
  if (async) {
    Downloader *dl = new Downloader(path, parent);
    dl->get(req, async, retries);
    ret = dl->state() != Downloader::Error;
  } else {
    Downloader dl(path);
    dl.get(req, async, retries);
    ret = dl.state() != Downloader::Error;
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
dlget(const QString &path, const QUrl &url, const QString &header,
      bool async, int retries, QObject *parent)
{
  DOUT("enter: path =" << path << ", url =" << url << ", async =" << async << ", retries =" << retries);
  bool ret = false;
  if (async) {
    Downloader *dl = new Downloader(path, parent);
    dl->get(url, header, async, retries);
    ret = dl->state() != Downloader::Error;
  } else {
    Downloader dl(path);
    dl.get(url, header, async, retries);
    ret = dl.state() != Downloader::Error;
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
dlpost(const QString &path, const QUrl &url,
       const QByteArray &data, const QString &header,
       bool async, int retries, QObject *parent)
{
  DOUT("enter: path =" << path << ", url =" << url << ", header =" << header << ", async =" << async << ", retries =" << retries);
  bool ret = false;
  if (async) {
    Downloader *dl = new Downloader(path, parent);
    dl->post(url, data, header, async, retries);
    ret = dl->state() != Downloader::Error;
  } else {
    Downloader dl(path);
    dl.post(url, data, header, async, retries);
    ret = dl.state() != Downloader::Error;
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

// EOF
