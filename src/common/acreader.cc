// acreader.cc
// 9/12/2012

#include "src/common/acreader.h"
#include "src/common/acglobal.h"

#define APP_NAME        AC_READER
#define APP_ID          AC_READER_ID

#define DEBUG "acreader"
#include "qtx/qxdebug.h"

// - Server -

AcReaderServer::AcReaderServer(QObject *parent)
  : Base(parent, APP_ID)
{ }

void
AcReaderServer::start()
{
#ifdef WITH_LIB_METACALL
  Base::startServer();
#endif // WITH_LIB_METACALL
}

void
AcReaderServer::stop()
{
#ifdef WITH_LIB_METACALL
  Base::stop();
#endif // WITH_LIB_METACALL
}

// - Client -

AcReader::AcReader(QObject *parent)
  : Base(parent), delegate_(0)
{
#ifdef WITH_LIB_METACALL
  // It is essential that the queued signal is passed to delegate_ from another object
  delegate_ = new Delegate(0, APP_ID);
  connect(this, SIGNAL(arguments(QStringList)),
          delegate_, SIGNAL(arguments(QStringList)), Qt::QueuedConnection);
#endif // WITH_LIB_METACALL
}

bool
AcReader::isRunning() const
{ return Delegate::isProcessRunning(APP_NAME); }

void
AcReader::open()
{ Delegate::open(APP_NAME); }

void
AcReader::openArguments(const QStringList &args)
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
