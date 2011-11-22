// module/serveragent/serveragent.cc
// 9/9/2011

#include "serveragent.h"
#include "serverproxy.h"
#include "serverproxy_config.h"
#include "castmanager.h"
#ifdef USE_MODULE_CLIENTAGENT
  #include "module/clientagent/clientagent.h"
#endif // USE_MODULE_CLIENTAGENT
#include <QtCore>

#define DEBUG "ServerAgent"
#include "module/debug/debug.h"

using namespace Core::Cloud;

#define CALLBACK_RETRY_TIME     5000 // in msec

// - Construction -

ServerAgent::ServerAgent(QObject *parent)
  : Base(parent), authorized_(false), connected_(false)
#ifdef USE_MODULE_CLIENTAGENT
  , client_(0)
#endif // USE_MODULE_CLIENTAGENT
{
  proxy_ = new ServerProxy(this);
  cast_ = new CastManager(this);

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
    bool ok = proxy_->isClientUpdated(CLIENT_TYPE);
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
  DOUT("login: mutex locking");
  mutex_.lock();
  DOUT("login: mutex locked");
  emit loginRequested(userName);

  bool updated = isSoftwareUpdated();
#ifndef DEBUG
  Q_UNUSED(updated);
#endif DEBUG
  DOUT("login: isSoftwareUpdated =" << updated);

  user_ = proxy_->selectUser(userName, passwordDigest);
  user_.setName(userName);
  user_.setPassword(passwordDigest);

  authorized_ = user_.isValid();

  if (authorized_) {
    //DOUT("login:update my user");
    //user_ = proxy_->getUser();
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
  DOUT("login: mutex unlocking");
  mutex_.unlock();
  DOUT("login: mutex unlocked");
  return authorized_;
}

void
ServerAgent::login()
{ login(user_.name(), user_.password()); }

void
ServerAgent::logout()
{
  DOUT("logout:enter");
  DOUT("logout: mutex locking");
  //mutex_.lock();
  DOUT("logout: mutex locked");
  emit logoutRequested();
  authorized_ = false;
  //proxy_->logout();
  emit logoutFinished();
  emit userChanged();
  DOUT("logout: mutex unlocking");
  //mutex_.unlock();
  DOUT("logout: mutex unlocked");
  DOUT("logout:exit");
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
{
  authorized_ = user_.hasName() && user_.hasPassword() &&
                proxy_->selectUser(user_.name(), user_.password()).isValid();
}

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
  if (!isAuthorized())
    return false;
  if (t == user_.isAnonymous())
    return true;
  bool ret = proxy_->setUserAnonymous(t, user_.name(), user_.password());
  if (ret)
    user_.setAnonymous(t);
  return ret;
}

bool
ServerAgent::setUserLanguage(qint32 language)
{
  if (!isAuthorized())
    return false;
  if (!language)
    return false;
  if (language == user_.language())
    return true;
  bool ret = proxy_->setUserLanguage(language, user_.name(), user_.password());
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
  DOUT("submitToken:enter: tt =" << token.type());
  if (!isAuthorized()) {
    DOUT("submitToken:exit: not authorized");
    return 0;
  }
  qint64 ret = proxy_->submitTokenDigest(token.digest(), token.digestType(), token.type(), user_.name(), user_.password());
  DOUT("submitToken:exit: ret =" << ret);
  return ret;
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
  DOUT("submitAlias:enter: tid =" << alias.tokenId());
  if (!isAuthorized()) {
    DOUT("submitAlias:exit: not authorized");
    return 0;
  }
  if (!alias.hasText()) {
    DOUT("submitAlias:exit: missing text");
    return 0;
  }
  qint64 ret = 0;
  if (alias.hasTokenId())
    ret = proxy_->submitAliasTextWithTokenId(alias.text(), alias.type(), alias.tokenId(), user_.name(), user_.password());
  else if (alias.hasTokenDigest())
    ret = proxy_->submitAliasTextAndTokenDigest(alias.text(), alias.type(), alias.tokenDigest(), alias.tokenDigestType(), user_.name(), user_.password());

  DOUT("submitAlias:exit: ret =" << ret);
  return ret;
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
  DOUT("submitAnnotation:enter: tid =" << annot.tokenId());
  if (!isAuthorized()) {
    DOUT("submitAnnotation:exit: not authorized");
    return 0;
  }
  if (!annot.hasText()) {
    DOUT("submitAnnotation:exit: missing text");
    return 0;
  }
  qint64 ret = 0;
  if (annot.hasTokenId())
    ret = proxy_->submitAnnotationTextWithTokenId(annot.text(), annot.pos(), annot.posType(), annot.tokenId(), user_.name(), user_.password());
  else if (annot.hasTokenDigest())
    ret = proxy_->submitAnnotationTextAndTokenDigest(annot.text(), annot.pos(), annot.posType(), annot.tokenDigest(), annot.tokenDigestType(), user_.name(), user_.password());
  DOUT("submitAnnotation:exit: ret =" << ret);
  return ret;
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
ServerAgent::selectTokenWithId(qint64 id)
{
  Token ret;
  if (id)
    ret = proxy_->selectTokenWithId(id);

  return ret;
}

Token
ServerAgent::selectTokenWithDigest(const QString &digest, qint32 digestType)
{
  Token ret;
  if (!digest.isEmpty())
    ret = proxy_->selectTokenWithDigest(digest, digestType);
  return ret;
}

AnnotationList
ServerAgent::selectAnnotationsWithTokenId(qint64 tid)
{
  AnnotationList ret;
  if (tid)
    ret = proxy_->selectAnnotationsWithTokenId(tid);
  return ret;
}

AnnotationList
ServerAgent::selectRelatedAnnotationsWithTokenId(qint64 tid)
{
  AnnotationList ret;
  if (tid)
    ret = proxy_->selectRelatedAnnotationsWithTokenId(tid);
  return ret;
}


AliasList
ServerAgent::selectAliasesWithTokenId(qint64 tid)
{
  AliasList ret;
  if (tid)
    ret = proxy_->selectAliasesWithTokenId(tid);
  return ret;
}

// - Cast -

#define DO_CAST(_cast, _Cast, _Entity) \
  bool \
  ServerAgent::_cast##_Entity##WithId(qint64 id) \
  { \
    if (!isAuthorized() || !id) \
      return false; \
 \
    CastEvent e(CastEvent::_Cast##Event, id, Traits::_Entity); \
    return cast_->registerEvent(e) && \
           proxy_->_cast##_Entity##WithId(id, user_.name(), user_.password()); \
  }

  DO_CAST(bless, Bless, Annotation)
  DO_CAST(curse, Curse, Annotation)
  DO_CAST(block, Block, Annotation)

  DO_CAST(bless, Bless, Alias)
  DO_CAST(curse, Curse, Alias)
  DO_CAST(block, Block, Alias)

  DO_CAST(bless, Bless, Token)
  DO_CAST(curse, Curse, Token)

  DO_CAST(block, Block, User)
#undef DO_CAST

#define QUERY_CAST(_Cast, _Casted, _Entity) \
  bool \
  ServerAgent::is##_Entity##_Casted##WithId(qint64 id) const \
  { \
    CastEvent e(CastEvent::_Cast##Event, id, Traits::_Entity); \
    return cast_->containsEvent(e); \
  }

  QUERY_CAST(Bless, Blessed, Annotation)
  QUERY_CAST(Curse, Cursed, Annotation)
  QUERY_CAST(Block, Blocked, Annotation)

  QUERY_CAST(Bless, Blessed, Alias)
  QUERY_CAST(Curse, Cursed, Alias)
  QUERY_CAST(Block, Blocked, Alias)

  QUERY_CAST(Bless, Blessed, Token)
  QUERY_CAST(Curse, Cursed, Token)

  QUERY_CAST(Block, Blocked, User)
#undef QUERY_CAST

// - Edit -

bool
ServerAgent::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  if (!isAuthorized())
    return 0;
  if (!id)
    return false;

  return proxy_->updateAnnotationTextWithId(text, id, user_.name(), user_.password());
}

// EOF
