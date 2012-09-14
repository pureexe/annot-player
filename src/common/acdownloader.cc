// acdownloader.cc
// 4/9/2012

#include "src/common/acdownloader.h"
#include "src/common/acglobal.h"

#define APP_NAME        AC_DOWNLOADER
#define APP_ID          AC_DOWNLOADER_ID

#define DEBUG "acdownloader"
#include "lib/debug/debug.h"

// - Server -

AcDownloaderServer::AcDownloaderServer(QObject *parent)
  : Base(parent, APP_ID)
{ }

void
AcDownloaderServer::start()
{
#ifdef WITH_LIB_METACALL
  Base::startServer();
#endif // WITH_LIB_METACALL
}

void
AcDownloaderServer::stop()
{
#ifdef WITH_LIB_METACALL
  Base::stop();
#endif // WITH_LIB_METACALL
}

// - Client -

AcDownloader::AcDownloader(QObject *parent)
  : Base(parent), delegate_(0)
{
#ifdef WITH_LIB_METACALL
  // It is essential that the queued signal is passed to delegate_ from another object
  delegate_ = new Delegate(this, APP_ID);
  connect(this, SIGNAL(arguments(QStringList)),
          delegate_, SIGNAL(arguments(QStringList)), Qt::QueuedConnection);
  connect(this, SIGNAL(showRequested()),
          delegate_, SIGNAL(showRequested()), Qt::QueuedConnection);
#endif // WITH_LIB_METACALL
}

bool
AcDownloader::isRunning() const
{ return Delegate::isProcessRunning(APP_NAME); }

void
AcDownloader::open()
{ Delegate::open(APP_NAME); }

void
AcDownloader::show()
{
  DOUT("enter");
#ifdef WITH_LIB_METACALL
  if (isRunning()) {
    DOUT("isRunning = true");
    if (!delegate_->isActive())
      delegate_->startClient();
    emit showRequested();
  } else
#endif // WITH_LIB_METACALL
  Delegate::open(APP_NAME);
  DOUT("exit");
}

void
AcDownloader::openArguments(const QStringList &args)
{
  DOUT("enter");
#ifdef WITH_LIB_METACALL
  if (isRunning()) {
    DOUT("isRunning = true");
    if (!delegate_->isActive())
      delegate_->startClient();
    emit arguments(args);
  } else
#endif // WITH_LIB_METACALL
  Delegate::open(APP_NAME, args);
  DOUT("exit");
}

// EOF
