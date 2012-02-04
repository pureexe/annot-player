// download.cc
// 2/4/2012

#include "download.h"
#include "downloader.h"
#include <QtCore>
#include <QtNetwork>

#define DEBUG "download"
#include "module/debug/debug.h"

bool
download(const QString &url, const QString &path, bool zipped, bool async)
{
  DOUT("enter: url =" << url << ", path =" << path << ", zipped =" << zipped << ", async =" << async);
  Downloader d(path, zipped);
  d.start(QUrl(url), async);
  bool ret = d.state() != Downloader::Error;
  DOUT("exit: ret =" << ret << ", state =" << d.state());
  return ret;
}

// EOF
