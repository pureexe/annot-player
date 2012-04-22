// acdownloader.cc
// 4/9/2012

#include "ac/acdownloader.h"
#include "ac/acglobal.h"

#define APP_NAME        AC_DOWNLOADER
#define APP_ID          AC_DOWNLOADER_ID

#define DEBUG "acdownloader"
#include "module/debug/debug.h"

// - Server -

AcDownloaderServer::AcDownloaderServer(QObject *parent)
  : Base(parent, APP_ID)
{ }

void
AcDownloaderServer::start()
{
#ifdef WITH_MODULE_METACALL
  Base::startServer();
#endif // WITH_MODULE_METACALL
}

void
AcDownloaderServer::stop()
{
#ifdef WITH_MODULE_METACALL
  Base::stop();
#endif // WITH_MODULE_METACALL
}

// - Client -

AcDownloader::AcDownloader(QObject *parent)
  : Base(parent), delegate_(0)
{
#ifdef WITH_MODULE_METACALL
  // It is essential that the queued signal is passed to delegate_ from another object
  delegate_ = new Delegate(this, APP_ID);
  connect(this, SIGNAL(arguments(QStringList)),
          delegate_, SIGNAL(arguments(QStringList)), Qt::QueuedConnection);
#endif // WITH_MODULE_METACALL
}

bool
AcDownloader::isRunning() const
{ return Delegate::isProcessRunning(APP_NAME); }

void
AcDownloader::open()
{ Delegate::open(APP_NAME); }

void
AcDownloader::openArguments(const QStringList &args)
{
  DOUT("enter");
#ifdef WITH_MODULE_METACALL
  if (isRunning()) {
    DOUT("isRunning = true");
    if (!delegate_->isActive())
      delegate_->startClient();
    emit arguments(args);
  } else
#endif // WITH_MODULE_METACALL
  Delegate::open(APP_NAME, args);
  DOUT("exit");
}

// EOF
