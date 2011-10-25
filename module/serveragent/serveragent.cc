// module/serveragent/serveragent.cc
// 9/9/2011

#include "serveragent.h"
#include "serverproxy.h"
#ifdef USE_MODULE_CLIENTAGENT
  #include "module/clientagent/clientagent.h"
#endif // USE_MODULE_CLIENTAGENT
#include <QtCore>

//#define DEBUG "ServerAgent"
#include "module/debug/debug.h"

#define CLIENT_TYPE "player" "-" "0.0.5.0"

using namespace Core::Data;

#define CALLBACK_RETRY_TIME     5000 // in msec

// - Construction -

ServerAgent::ServerAgent(QObject *parent)
  : Base(parent), authorized_(false), connected_(false)
#ifdef USE_MODULE_CLIENTAGENT
  , client_(0)
#endif // USE_MODULE_CLIENTAGENT
{
  proxy_ = new ServerProxy(this);

  connect(proxy_, SIGNAL(soapError(int)), SLOT(reportSoapError(int)));
}

#ifdef USE_MODULE_CLIENTAGENT
void
ServerAgent::setClientAgent(ClientAgent *client)
{ client_ = client; }
#endif // USE_MODULE_CLIENTAGENT

void
ServerAgent::reportSoapError(int err)
{
  DOUT("reportSoapError:enter: err =" << err);
  switch (err) {
  case 28:      emit connectionError(); break;
  case 404:     emit error404(); break;
  case 500:     emit serverError(); break;
  default:      emit unknownError();
  }
  DOUT("reportSoapError:exit");
}

// - Login -

bool
ServerAgent::isConnected() const
{ return connected_; }

bool
ServerAgent::isAuthorized() const
{ return authorized_; }

const User&
ServerAgent::user() const
{ return user_; }

User&
ServerAgent::user()
{ return user_; }

bool
ServerAgent::isSoftwareUpdated()
{
  static int ret = -1;
  if (ret < 0) {
    bool ok = proxy_->setClientType(CLIENT_TYPE);
    ret = ok ? 1 : 0;
  }
  return ret;
}

void
ServerAgent::setUser(const User &user)
{
  user_ = user;
  emit userChanged();
}

bool
ServerAgent::login(const QString &userName, const QString &passwordDigest)
{
  DOUT("login:enter");
  emit loginRequested(userName);
  user_.setName(userName);
  user_.setPassword(passwordDigest);
  authorized_ = proxy_->login(userName, passwordDigest);

  if (authorized_) {
    DOUT("login:update my user");
    user_ = proxy_->getUser();
    //Q_ASSERT(user_.id());
    DOUT("login: new user id =" << user_.id());

#ifdef USE_MODULE_CLIENTAGENT
    if (client_ && client_->isReady()) {
      DOUT("login: client service is ready, try setCallback");
      proxy_->setCallback(client_->port(), client_->nextPublicKey());
      // TODO: try call back again in 5 secs
    } else
      DOUT("login: client service is not ready, skip setCallback TODO!!!!!");
#endif // USE_MODULE_CLIENTAGENT

    emit loginSucceeded(userName);
  } else
    emit loginFailed(userName);
  DOUT("login:exit: ret =" << authorized_);
  emit userChanged();
  return authorized_;
}

void
ServerAgent::login()
{ login(user_.name(), user_.password()); }

void
ServerAgent::logout()
{
  emit logoutRequested();
  authorized_ = false;
  proxy_->logout();
  emit logoutFinished();
  emit userChanged();
}

void
ServerAgent::setAuthorized(bool yes)
{ authorized_ = yes; }

void
ServerAgent::setConnected(bool yes)
{ connected_ = yes; }

void
ServerAgent::disconnect()
{ setConnected(false); }

void
ServerAgent::updateAuthorized()
{ authorized_ = proxy_->getAuthorized(); }

void
ServerAgent::updateConnected()
{
  connected_ = proxy_->getConnected();
  if (connected_)
    emit connected();
}

// - User -

bool
ServerAgent::setUserAnonymous(bool t)
{
  if (t == user_.isAnonymous())
    return true;
  bool ret = proxy_->setUserAnonymous(t);
  if (ret)
    user_.setAnonymous(t);
  return ret;
}

bool
ServerAgent::setUserLanguage(qint32 language)
{
  if (!language)
    return false;
  if (language == user_.language())
    return true;
  bool ret = proxy_->setUserLanguage(language);
  if (ret)
    user_.setLanguage(language);
  return ret;
}

// - Chat -
QString
ServerAgent::chat(const QString &message)
{ return proxy_->chat(message); }

// - Queries -

qint64
ServerAgent::submitToken(const Token &token)
{
  switch (token.type()) {
  case Traits::MediaType: return proxy_->submitMediaTokenDigest(token.digest());
  case Traits::GameType:  return proxy_->submitGameTokenDigest(token.digest());
  default: Q_ASSERT(0); return 0;
  }
}

bool
ServerAgent::submitTokens(const TokenList &tokens)
{
  bool ret = true;
  foreach (const Token &t, tokens)
    ret = ret && submitToken(t);
  return ret;
}

qint64
ServerAgent::submitAlias(const Alias &alias)
{
  if (alias.hasId())
    switch (alias.type()) {
    case Traits::MediaType: return proxy_->submitMediaAliasTextWithTokenId(alias.text(), alias.aliasType(), alias.tokenId());
    case Traits::GameType:  return proxy_->submitGameAliasTextWithTokenId(alias.text(), alias.aliasType(), alias.tokenId());
    default: Q_ASSERT(0); return 0;
    }
  else if (alias.hasTokenDigest())
    switch (alias.type()) {
    case Traits::MediaType: return proxy_->submitMediaAliasTextAndTokenDigest(alias.text(), alias.aliasType(), alias.tokenDigest());
    case Traits::GameType:  return proxy_->submitGameAliasTextAndTokenDigest(alias.text(), alias.aliasType(), alias.tokenDigest());
    default: Q_ASSERT(0); return 0;
    }
  else
    return 0;
}

bool
ServerAgent::submitAliases(const AliasList &aliases)
{
  bool ret = true;
  foreach (const Alias &a, aliases)
    ret = ret && submitAlias(a);
  return ret;
}

qint64
ServerAgent::submitTokenAndAlias(const Token &token, const Alias &alias)
{
  qint64 tid = submitToken(token);
  if (tid) {
    Alias a = alias;
    a.setTokenId(tid);
    submitAlias(a);
  }
  return tid;
}

qint64
ServerAgent::submitAnnotation(const Annotation &annot)
{
  if (annot.hasId())
    switch (annot.type()) {
    case Traits::MediaType: return proxy_->submitMediaAnnotationTextWithTokenId(annot.text(), annot.pos(), annot.posType(), annot.tokenId());
    case Traits::GameType:  return proxy_->submitGameAnnotationTextWithTokenId(annot.text(), annot.pos(), annot.posType(), annot.tokenId());
    default: Q_ASSERT(0); return 0;
    }
  else if (annot.hasTokenDigest())
    switch (annot.type()) {
    case Traits::MediaType: return proxy_->submitMediaAnnotationTextAndTokenDigest(annot.text(), annot.pos(), annot.posType(), annot.tokenDigest());
    case Traits::GameType:  return proxy_->submitGameAnnotationTextAndTokenDigest(annot.text(), annot.pos(), annot.posType(), annot.tokenDigest());
    default: Q_ASSERT(0); return 0;
    }
  else
    return 0;
}

bool
ServerAgent::submitAnnotations(const AnnotationList &tokens)
{
  bool ret = true;
  foreach (const Annotation &a, tokens)
    ret = ret && submitAnnotation(a);
  return ret;
}

Token
ServerAgent::selectTokenWithId(qint64 id, int tt)
{
  Token ret;
  if (!id)
    return ret;

  switch (tt) {
  case Traits::MediaType: ret = proxy_->selectMediaTokenWithId(id); break;
  case Traits::GameType:  ret = proxy_->selectGameTokenWithId(id); break;
  default: Q_ASSERT(0);
  }
  return ret;
}

Token
ServerAgent::selectTokenWithDigest(const QString &digest, int tt)
{
  Token ret;
  if (digest.isEmpty())
    return ret;

  switch (tt) {
  case Traits::MediaType: ret = proxy_->selectMediaTokenWithDigest(digest); break;
  case Traits::GameType:  ret = proxy_->selectGameTokenWithDigest(digest); break;
  default: Q_ASSERT(0);
  }
  return ret;
}

AnnotationList
ServerAgent::selectAnnotationsWithTokenId(qint64 tid, int tt)
{
  AnnotationList ret;
  if (!tid)
    return ret;

  switch (tt) {
  case Traits::MediaType: ret = proxy_->selectMediaAnnotationsWithTokenId(tid); break;
  case Traits::GameType:  ret = proxy_->selectGameAnnotationsWithTokenId(tid); break;
  default: Q_ASSERT(0);
  }
  return ret;
}

AnnotationList
ServerAgent::selectRelatedAnnotationsWithTokenId(qint64 tid, int tt)
{
  AnnotationList ret;
  if (!tid)
    return ret;

  switch (tt) {
  case Traits::MediaType: ret = proxy_->selectRelatedMediaAnnotationsWithTokenId(tid); break;
  case Traits::GameType:  ret = proxy_->selectRelatedGameAnnotationsWithTokenId(tid); break;
  default: Q_ASSERT(0);
  }
  return ret;
}


AliasList
ServerAgent::selectAliasesWithTokenId(qint64 tid, int tt)
{
  AliasList ret;
  if (!tid)
    return ret;

  switch (tt) {
  case Traits::MediaType: ret = proxy_->selectMediaAliasesWithTokenId(tid); break;
  case Traits::GameType:  ret = proxy_->selectGameAliasesWithTokenId(tid); break;
  default: Q_ASSERT(0);
  }
  return ret;
}

// - Cast -

bool
ServerAgent::blessAnnotationWithId(qint64 id, int tt)
{
  if (!id)
    return false;

  switch (tt) {
  case Traits::MediaType: return proxy_->blessMediaAnnotationWithId(id);
  case Traits::GameType:  return proxy_->blessGameAnnotationWithId(id);
  default: Q_ASSERT(0); return false;
  }
}

// - Edit -

bool
ServerAgent::updateAnnotationTextWithId(const QString &text, qint64 id, int tt)
{
  if (!id)
    return false;

  switch (tt) {
  case Traits::MediaType: return proxy_->updateMediaAnnotationTextWithId(text, id);
  case Traits::GameType:  return proxy_->updateGameAnnotationTextWithId(text, id);
  default: Q_ASSERT(0); return false;
  }
}

// EOF
