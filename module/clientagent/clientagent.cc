// module/clientagent/clientagent.cc
// 10/6/2011

#include "clientagent.h"
#include "clientservice.h"
#include "clientservice_config.h"
#ifdef WITH_MODULE_SERVERAGENT
# include "module/serveragent/serveragent.h"
#endif // WITH_MODULE_SERVERAGENT
#include <QtCore>

#define DEBUG "clientagent"
#include "module/debug/debug.h"

using namespace Core::Universe;

// - Construction -

ClientAgent::ClientAgent(QObject *parent)
  : Base(parent), authorized_(false), connected_(false)
#ifdef WITH_MODULE_SERVERAGENT
  , server_(0)
#endif // WITH_MODULE_SERVERAGENT
{
  DOUT("enter");
  qsrand(QDateTime::currentMSecsSinceEpoch());
  randomizePrivateKey();

  DOUT("try start service");
  service_ = new ClientService(this);
  service_->setDelegate(this);

  randomizeServicePort();
  DOUT("exit");

  service_->start();
}

#ifdef WITH_MODULE_SERVERAGENT
void
ClientAgent::setServerAgent(ServerAgent *server)
{ server_ = server; }
#endif // WITH_MODULE_SERVERAGENT

long
ClientAgent::privateKey() const
{ return key_; }

void
ClientAgent::setPrivateKey(long key)
{ key_ = key; }

// TODO
long
ClientAgent::nextPublicKey() const
{ return privateKey(); }

void
ClientAgent::randomizePrivateKey()
{ key_ = key_ * 13 + qrand(); }

int
ClientAgent::port() const
{ return service_->port(); }

void
ClientAgent::setPort(int port)
{ return service_->setPort(port); }

void
ClientAgent::randomizeServicePort()
{
  int range = CLIENT_SERVICE_MAX_PORT - CLIENT_SERVICE_MIN_PORT;
  int port =  qrand() % range  + CLIENT_SERVICE_MIN_PORT;

  setPort(port);
}

// - Service -

bool
ClientAgent::isReady() const
{ return service_->isRunning(); }

// - Override methods -

bool
ClientAgent::authorize(long key)
{
  bool ret = (key_ == key);
  setAuthorized(ret);
  if (ret)
    emit authorized();
  else
    emit authorizationError();
  return ret;
}

void
ClientAgent::deauthorize()
{
  setAuthorized(false);
  emit deauthorized();
}

bool
ClientAgent::isAuthorized() const
{ return authorized_; }

void
ClientAgent::setAuthorized(bool t)
{ authorized_ = t; }

bool
ClientAgent::isConnected() const
{ return connected_; }

void
ClientAgent::setConnected(bool t)
{ connected_ = t ; }

void
ClientAgent::chat(const QString &text)
{ emit chatReceived(text); }

/*
void
ClientAgent::reportSoapError(int err)
{
  DOUT("enter: err =" << err);
  switch (err) {
  case 28:      emit connectionError(); break;
  case 404:     emit error404(); break;
  case 500:     emit clientError(); break;
  default:      emit unknownError();
  }
  DOUT("exit");
}

// - Login -

bool
ClientAgent::isConnected() const
{ return connected_; }

bool
ClientAgent::isAuthorized() const
{ return authorized_; }

const User&
ClientAgent::user() const
{ return user_; }

void
ClientAgent::setUser(const User &user)
{
  user_ = user;
  emit userChanged();
}

bool
ClientAgent::login(const QString &userName, const QString &passwordDigest)
{
  DOUT("enter");
  emit loginRequested(userName);
  user_.setName(userName);
  user_.setPassword(passwordDigest);
  authorized_ = delegate_->login(userName, passwordDigest);

  if (authorized_) {
    DOUT("update my user");
    user_ = delegate_->getUser();
    //Q_ASSERT(user_.id());
    DOUT("new user id =" << user_.id());
    emit loginSucceeded(userName);
  } else
    emit loginFailed(userName);
  DOUT("exit: ret =" << authorized_);
  emit userChanged();
  return authorized_;
}

void
ClientAgent::login()
{ login(user_.name(), user_.password()); }

void
ClientAgent::logout()
{
  emit logoutRequested();
  authorized_ = false;
  delegate_->logout();
  emit logoutFinished();
  emit userChanged();
}

void
ClientAgent::setAuthorized(bool yes)
{ authorized_ = yes; }

void
ClientAgent::setConnected(bool yes)
{ connected_ = yes; }

void
ClientAgent::disconnect()
{ setConnected(false); }

void
ClientAgent::updateAuthorized()
{ authorized_ = delegate_->getAuthorized(); }

void
ClientAgent::updateConnected()
{
  connected_ = delegate_->getConnected();
  if (connected_)
    emit connected();
}

// - Chat -
QString
ClientAgent::chat(const QString &message)
{ return delegate_->chat(message); }

*/


// EOF
