// remotestream.cc
// 2/15/2012

#include "lib/stream/remotestream.h"
#include <QtCore/QEventLoop>
#include <QtNetwork/QNetworkAccessManager>

// - Wait -

void
RemoteStream::waitForReady()
{
  enum { max_redirect = 5 };
  int retries = max_redirect;
  do waitForReadyRead();
  while (retries--  && tryRedirect());
}

void
RemoteStream::waitForReadyRead()
{
  QEventLoop loop;
  connect(this, SIGNAL(errorMessage(QString)), &loop, SLOT(quit()));
  connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
  connect(this, SIGNAL(readyRead()), &loop, SLOT(quit()));
  connect(this, SIGNAL(stopped()), &loop, SLOT(quit()));
  loop.exec();
}

// EOF
