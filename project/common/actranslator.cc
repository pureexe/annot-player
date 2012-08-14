// actranslator.cc
// 8/11/2012

#include "project/common/actranslator.h"
#include "project/common/acglobal.h"

#define APP_NAME        AC_TRANSLATOR
#define APP_ID          AC_TRANSLATOR_ID

#define DEBUG "actranslator"
#include "module/debug/debug.h"

// - Server -

AcTranslatorServer::AcTranslatorServer(QObject *parent)
  : Base(parent, APP_ID)
{ }

void
AcTranslatorServer::start()
{
#ifdef WITH_MODULE_METACALL
  Base::startServer();
#endif // WITH_MODULE_METACALL
}

void
AcTranslatorServer::stop()
{
#ifdef WITH_MODULE_METACALL
  Base::stop();
#endif // WITH_MODULE_METACALL
}

// - Client -

AcTranslator::AcTranslator(QObject *parent)
  : Base(parent), delegate_(0)
{
#ifdef WITH_MODULE_METACALL
  // It is essential that the queued signal is passed to delegate_ from another object
  delegate_ = new Delegate(this, APP_ID);
  connect(this, SIGNAL(arguments(QStringList)),
          delegate_, SIGNAL(arguments(QStringList)), Qt::QueuedConnection);
  connect(this, SIGNAL(showRequested()),
          delegate_, SIGNAL(showRequested()), Qt::QueuedConnection);
#endif // WITH_MODULE_METACALL
}

bool
AcTranslator::isRunning() const
{ return Delegate::isProcessRunning(APP_NAME); }

void
AcTranslator::open()
{ Delegate::open(APP_NAME); }

void
AcTranslator::show()
{
  DOUT("enter");
#ifdef WITH_MODULE_METACALL
  if (isRunning()) {
    DOUT("isRunning = true");
    if (!delegate_->isActive())
      delegate_->startClient();
    emit showRequested();
  } else
#endif // WITH_MODULE_METACALL
  Delegate::open(APP_NAME);
  DOUT("exit");
}

void
AcTranslator::openArguments(const QStringList &args)
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
