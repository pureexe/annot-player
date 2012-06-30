// module/serveragent/serveragent.cc
// 9/9/2011

#include "module/serveragent/serveragent.h"
#include "module/serveragent/serverproxy.h"
#include "module/serveragent/serverproxy_config.h"
#include "module/serveragent/castmanager.h"
#ifdef WITH_MODULE_CLIENTAGENT
# include "module/clientagent/clientagent.h"
#endif // WITH_MODULE_CLIENTAGENT
#include <QtCore/QMutexLocker>

//#define DEBUG "serveragent"
#include "module/debug/debug.h"

using namespace AnnotCloud;

#define CALLBACK_RETRY_TIME     5000 // in msec

// - Construction -

ServerAgent::ServerAgent(QObject *parent)
  : Base(parent), authorized_(false), connected_(false)
#ifdef WITH_MODULE_CLIENTAGENT
  , client_(0)
#endif // WITH_MODULE_CLIENTAGENT
{
  proxy_ = new ServerProxy(this);
  cast_ = new CastManager(this);

  connect(proxy_, SIGNAL(soapError(int)), SLOT(reportSoapError(int)));
}

#ifdef WITH_MODULE_CLIENTAGENT
void
ServerAgent::setClientAgent(ClientAgent *client)
{ client_ = client; }
#endif // WITH_MODULE_CLIENTAGENT

void
ServerAgent::reportSoapError(int err)
{
  DOUT("enter: err =" << err);
  switch (err) {
  case 28:      emit connectionError(); break;
  case 404:     emit error404(); break;
  case 500:     emit serverError(); break;
  default:      emit unknownError();
  }
  DOUT("exit");
}

// - Login -

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
  DOUT("enter");
  QMutexLocker lock(&mutex_);
  emit loginRequested(userName);

  bool updated = isSoftwareUpdated();
#ifndef DEBUG
  Q_UNUSED(updated);
#endif // DEBUG
  DOUT("isSoftwareUpdated =" << updated);

  user_ = proxy_->selectUser(userName, passwordDigest);
  user_.setName(userName);
  user_.setPassword(passwordDigest);

  authorized_ = user_.isValid();

  if (authorized_) {
    //DOUT("update my user");
    //user_ = proxy_->getUser();
    //Q_ASSERT(user_.id());
    DOUT("new user id =" << user_.id());

#ifdef WITH_MODULE_CLIENTAGENT
    if (client_ && client_->isReady()) {
      DOUT("client service is ready, try setCallback");
      proxy_->setCallback(client_->port(), client_->nextPublicKey());
      // TODO: try call back again in 5 secs
    } else
      DOUT("client service is not ready, skip setCallback TODO!!!!!");
#endif // WITH_MODULE_CLIENTAGENT

    emit loginSucceeded(userName);
  } else
    emit loginFailed(userName);
  DOUT("exit: ret =" << authorized_);
  emit userChanged();
  return authorized_;
}

void
ServerAgent::login()
{ login(user_.name(), user_.password()); }

void
ServerAgent::logout()
{
  DOUT("enter");
  DOUT("mutex locking");
  //mutex_.lock();
  DOUT("mutex locked");
  emit logoutRequested();
  authorized_ = false;
  //proxy_->logout();
  emit logoutFinished();
  emit userChanged();
  DOUT("mutex unlocking");
  //mutex_.unlock();
  DOUT("mutex unlocked");
  DOUT("exit");
}

void
ServerAgent::setAuthorized(bool yes)
{ authorized_ = yes; }

void
ServerAgent::setConnected(bool t)
{
  if (connected_ != t)
    emit connectedChanged(connected_ = t);
}

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
  bool t = connected_;
  connected_ = proxy_->getConnected();
  if (connected_)
    emit connected();
  if (connected_ != t)
    emit connectedChanged(connected_);
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
ServerAgent::selectTokenId(const Token &token)
{
  DOUT("enter: tt =" << token.type());

  qint64 ret = 0;
  if (token.hasDigest())
    ret = proxy_->selectTokenIdWithDigest(token.digest(), token.part());
  else if (token.hasSource())
    ret = proxy_->selectTokenIdWithSource(token.source(), token.part());
  DOUT("exit: ret =" << ret);
  return ret;
}

qint64
ServerAgent::submitToken(const Token &token)
{
  DOUT("enter: tt =" << token.type());
  if (!isAuthorized()) {
    DOUT("exit: not authorized");
    return 0;
  }

  qint64 ret = 0;
  if (token.hasDigest())
    ret = proxy_->submitTokenDigest(token.digest(), token.part(), token.type(), user_.name(), user_.password());
  else if (token.hasSource())
    ret = proxy_->submitTokenSource(token.source(), token.part(), token.type(), user_.name(), user_.password());
  DOUT("exit: ret =" << ret);
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
  DOUT("enter: tid =" << alias.tokenId());
  if (!isAuthorized()) {
    DOUT("exit: not authorized");
    return 0;
  }
  if (!alias.hasText()) {
    DOUT("exit: missing text");
    return 0;
  }
  qint64 ret = 0;
  if (alias.hasTokenId())
    ret = proxy_->submitAliasTextWithTokenId(alias.text(), alias.type(), alias.tokenId(), user_.name(), user_.password());
  else if (alias.hasTokenDigest())
    ret = proxy_->submitAliasTextAndTokenDigest(alias.text(), alias.type(), alias.tokenDigest(), alias.tokenPart(), user_.name(), user_.password());

  DOUT("exit: ret =" << ret);
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
  DOUT("enter: tid =" << annot.tokenId() << ", uid =" << annot.userId());
  if (!isAuthorized()) {
    DOUT("exit: not authorized");
    return 0;
  }
  if (!annot.hasText()) {
    DOUT("exit: missing text");
    return 0;
  }
  if (annot.userId() <= 0 || user_.id() != annot.userId()) {
    DOUT("exit: mismatched userId");
    return 0;
  }
  qint64 ret = 0;
  if (annot.hasTokenId())
    ret = proxy_->submitAnnotationTextWithTokenId(annot.text(), annot.pos(), annot.posType(), annot.tokenId(), user_.name(), user_.password());
  else if (annot.hasTokenDigest())
    ret = proxy_->submitAnnotationTextAndTokenDigest(annot.text(), annot.pos(), annot.posType(), annot.tokenDigest(), annot.tokenPart(), user_.name(), user_.password());
  DOUT("exit: ret =" << ret);
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
ServerAgent::selectTokenWithDigest(const QString &digest, qint32 part)
{
  Token ret;
  if (!digest.isEmpty())
    ret = proxy_->selectTokenWithDigest(digest, part);
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
ServerAgent::selectRelatedAliasesWithTokenId(qint64 tid)
{
  AliasList ret;
  if (tid)
    ret = proxy_->selectRelatedAliasesWithTokenId(tid);
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

// - Deletion -

bool
ServerAgent::deleteAnnotationWithId(qint64 id)
{
  bool ok = false;
  if (id && isAuthorized())
    ok = proxy_->deleteAnnotationWithId(id, user_.name(), user_.password());
  return ok;
}

bool
ServerAgent::deleteAliasWithId(qint64 id)
{
  bool ok = false;
  if (id && isAuthorized())
    ok = proxy_->deleteAliasWithId(id, user_.name(), user_.password());
  return ok;
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

  DO_CAST(bless, Bless, User)
  DO_CAST(block, Block, User)
  DO_CAST(curse, Curse, User)
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

// - Live -

qint64
ServerAgent::submitLiveAnnotation(const Annotation &annot)
{
  if (!isAuthorized()) {
    DOUT("exit: not authorized");
    return 0;
  }
  if (!annot.hasUserId() || user_.id() != annot.userId()) {
    DOUT("exit: mismatched userId");
    return 0;
  }
  if (!annot.hasText()) {
    DOUT("exit: missing text");
    return 0;
  }

  qint64 ret = proxy_->submitLiveAnnotationTextWithTokenId(annot.text(), Token::TI_Public, user_.name(), user_.password());
  DOUT("exit: ret =" << ret);
  return ret;
}

qint64
ServerAgent::submitLiveAnnotationText(const QString &text)
{
  if (!isAuthorized()) {
    DOUT("exit: not authorized");
    return 0;
  }
  if (text.isEmpty()) {
    DOUT("exit: missing text");
    return 0;
  }

  qint64 ret = proxy_->submitLiveAnnotationTextWithTokenId(text, Token::TI_Public, user_.name(), user_.password());
  DOUT("exit: ret =" << ret);
  return ret;
}


AnnotationList
ServerAgent::selectLiveAnnotations()
{
  AnnotationList ret = proxy_->selectLiveAnnotationsWithTokenId(Token::TI_Public);
  return ret;
}

qint32
ServerAgent::selectLiveTokenInterval()
{
  qint32 ret = proxy_->selectLiveTokenIntervalWithId(Token::TI_Public);
  return ret;
}

// EOF
