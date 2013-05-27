// annotserveragent.cc
// 9/9/2011

#include "lib/annotservice/annotserveragent.h"
#include "lib/annotservice/annotserverproxy_p.h"
#include "lib/annotservice/castmanager_p.h"
#include "lib/annotservice/cloudservice_config.h"
#include <QtCore/QMutexLocker>
#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>

//#define DEBUG "annotserveragent"
#include "qtx/qxdebug.h"

using namespace AnnotCloud;

#define CALLBACK_RETRY_TIME     5000 // in msec

QString
AnnotationServerAgent::version()
{
  QSettings settings("etc/settings.ini", QSettings::IniFormat);
  settings.beginGroup("Application");
  return settings.value("Version").toString();
}

// - Construction -

AnnotationServerAgent::AnnotationServerAgent(QObject *parent)
  : Base(parent), authorized_(false), connected_(false)
{
  proxy_ = new AnnotationServerProxy(this);
  cast_ = new CastManager(this);

  connect(proxy_, SIGNAL(soapError(int)), SLOT(reportSoapError(int)));
}

void
AnnotationServerAgent::reportSoapError(int err)
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
AnnotationServerAgent::user() const
{ return user_; }

User&
AnnotationServerAgent::user()
{ return user_; }

bool
AnnotationServerAgent::isSoftwareUpdated()
{
  static int ret = -1;
  if (ret < 0) {
    bool ok = proxy_->isLatestApp(version());
    ret = ok ? 1 : 0;
  }
  return ret;
}

void
AnnotationServerAgent::setUser(const User &user)
{
  user_ = user;
  emit userChanged();
}

bool
AnnotationServerAgent::login(const QString &userName, const QString &passwordDigest)
{
  DOUT("enter");
  QMutexLocker lock(&mutex_);
  emit loginRequested(userName);

  bool updated = isSoftwareUpdated();
#ifndef DEBUG
  Q_UNUSED(updated)
#endif // DEBUG
  DOUT("isSoftwareUpdated =" << updated);

  bool ok = proxy_->login(userName, passwordDigest);
  if (ok) {
    user_ = proxy_->getUser();
    user_.setName(userName);
    user_.setPassword(passwordDigest);
  } else
    user_.setId(0);

  authorized_ = user_.isValid();

  if (authorized_) {
    //DOUT("update my user");
    //user_ = proxy_->getUser();
    //Q_ASSERT(user_.id());
    DOUT("new user id =" << user_.id());

    emit loginSucceeded(userName);
  } else
    emit loginFailed(userName);
  DOUT("exit: ret =" << authorized_);
  emit userChanged();
  return authorized_;
}

bool
AnnotationServerAgent::login()
{
  if (user_.hasName() && user_.hasPassword())
    return proxy_->login(user_.name(), user_.password());
  else
    return false;
}

void
AnnotationServerAgent::logout()
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
AnnotationServerAgent::setAuthorized(bool yes)
{ authorized_ = yes; }

void
AnnotationServerAgent::setConnected(bool t)
{
  if (connected_ != t)
    emit connectedChanged(connected_ = t);
}

void
AnnotationServerAgent::disconnect()
{ setConnected(false); }

void
AnnotationServerAgent::updateAuthorized()
{
  authorized_ = user_.hasName() && user_.hasPassword() &&
                proxy_->login(user_.name(), user_.password());
}

void
AnnotationServerAgent::updateConnected()
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
AnnotationServerAgent::setUserAnonymous(bool t)
{
  if (!isAuthorized())
    return false;
  if (t == user_.isAnonymous())
    return true;
  bool ret = proxy_->setUserAnonymous(t);
  if (ret)
    user_.setAnonymous(t);
  return ret;
}

bool
AnnotationServerAgent::setUserLanguage(qint32 language)
{
  if (!isAuthorized())
    return false;
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
AnnotationServerAgent::chat(const QString &message)
{ return proxy_->chat(message); }

// - Queries -

qint64
AnnotationServerAgent::selectTokenId(const Token &token)
{
  DOUT("enter: tt =" << token.type());

  qint64 ret = 0;
  if (token.hasDigest())
    ret = proxy_->selectTokenIdWithDigest(token.digest(), token.section());
  else if (token.hasUrl())
    ret = proxy_->selectTokenIdWithUrl(token.url(), token.section());
  DOUT("exit: ret =" << ret);
  return ret;
}

qint64
AnnotationServerAgent::submitGameHook(const GameHook &hook)
{
  DOUT("enter: text =" << hook.text());
  if (!isAuthorized()) {
    DOUT("exit: not authorized");
    return 0;
  }

  if (!hook.hasTokenId() || !hook.hasText()) {
    DOUT("exit: incomplete game hook");
    return 0;
  }

  qint64 ret = proxy_->submitGameHook(hook);
  if (!ret && login())
    ret = proxy_->submitGameHook(hook);
  DOUT("exit: ret =" << ret);
  return ret;
}

qint64
AnnotationServerAgent::submitGameThread(const GameThread &thread)
{
  DOUT("enter: type =" << thread.type());
  if (!isAuthorized()) {
    DOUT("exit: not authorized");
    return 0;
  }

  if (!thread.hasTokenId() || !thread.hasEncoding() || !thread.hasSignature() || !thread.hasProvider()) {
    DOUT("exit: incomplete game thread");
    return 0;
  }

  qint64 ret = proxy_->submitGameThread(thread);
  if (!ret && login())
    ret = proxy_->submitGameThread(thread);
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationServerAgent::submitGameHooks(const GameHookList &hooks)
{
  bool ret = true;
  foreach (const GameHook &h, hooks)
    ret = ret && submitGameHook(h);
  return ret;
}

bool
AnnotationServerAgent::submitGameThreads(const GameThreadList &threads)
{
  bool ret = true;
  foreach (const GameThread &t, threads)
    ret = ret && submitGameThread(t);
  return ret;
}

qint64
AnnotationServerAgent::submitToken(const Token &token)
{
  DOUT("enter: tt =" << token.type());
  if (!isAuthorized()) {
    DOUT("exit: not authorized");
    return 0;
  }

  qint64 ret = 0;
  if (token.hasDigest())
    ret = proxy_->submitTokenDigest(token.digest(), token.section(), token.type());
  else if (token.hasUrl())
    ret = proxy_->submitTokenUrl(token.url(), token.section(), token.type());

  if (!ret && login()) {
    if (token.hasDigest())
      ret = proxy_->submitTokenDigest(token.digest(), token.section(), token.type());
    else if (token.hasUrl())
      ret = proxy_->submitTokenUrl(token.url(), token.section(), token.type());
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationServerAgent::submitTokens(const TokenList &tokens)
{
  bool ret = true;
  foreach (const Token &t, tokens)
    ret = ret && submitToken(t);
  return ret;
}

qint64
AnnotationServerAgent::submitAlias(const Alias &alias)
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
    ret = proxy_->submitAliasTextWithTokenId(alias.text(), alias.type(), alias.tokenId());
  else if (alias.hasTokenDigest())
    ret = proxy_->submitAliasTextAndTokenDigest(alias.text(), alias.type(), alias.tokenDigest(), alias.tokenSection());

  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationServerAgent::submitAliases(const AliasList &aliases)
{
  bool ret = true;
  foreach (const Alias &a, aliases)
    ret = ret && submitAlias(a);
  return ret;
}

qint64
AnnotationServerAgent::submitTokenAndAlias(const Token &token, const Alias &alias)
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
AnnotationServerAgent::submitAnnotation(const Annotation &annot)
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
    ret = proxy_->submitAnnotationTextWithTokenId(annot.text(), annot.pos(), annot.posType(), annot.tokenId());
  else if (annot.hasTokenDigest())
    ret = proxy_->submitAnnotationTextAndTokenDigest(annot.text(), annot.pos(), annot.posType(), annot.tokenDigest(), annot.tokenSection());

  if (!ret && login()) {
    if (annot.hasTokenId())
      ret = proxy_->submitAnnotationTextWithTokenId(annot.text(), annot.pos(), annot.posType(), annot.tokenId());
    else if (annot.hasTokenDigest())
      ret = proxy_->submitAnnotationTextAndTokenDigest(annot.text(), annot.pos(), annot.posType(), annot.tokenDigest(), annot.tokenSection());
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationServerAgent::submitAnnotations(const AnnotationList &tokens)
{
  bool ret = true;
  foreach (const Annotation &a, tokens)
    ret = ret && submitAnnotation(a);
  return ret;
}

GameHook
AnnotationServerAgent::selectGameHookWithTokenId(qint64 tid)
{
  GameHook ret;
  if (tid)
    ret = proxy_->selectGameHookWithTokenId(tid);
  return ret;
}

GameHook
AnnotationServerAgent::selectGameHookWithTokenDigest(const QString &digest)
{ return proxy_->selectGameHookWithTokenDigest(digest); }

GameThread
AnnotationServerAgent::selectGameThreadWithTokenId(qint64 tid)
{
  GameThread ret;
  if (tid)
    ret = proxy_->selectGameThreadWithTokenId(tid);
  return ret;
}

GameThread
AnnotationServerAgent::selectGameThreadWithTokenDigest(const QString &digest)
{ return proxy_->selectGameThreadWithTokenDigest(digest); }

Token
AnnotationServerAgent::selectTokenWithId(qint64 id)
{
  Token ret;
  if (id)
    ret = proxy_->selectTokenWithId(id);

  return ret;
}

Token
AnnotationServerAgent::selectTokenWithDigest(const QString &digest, qint32 section)
{
  Token ret;
  if (!digest.isEmpty())
    ret = proxy_->selectTokenWithDigest(digest, section);
  return ret;
}

AnnotationList
AnnotationServerAgent::selectAnnotationsWithTokenId(qint64 tid)
{
  AnnotationList ret;
  if (tid)
    ret = proxy_->selectAnnotationsWithTokenId(tid);
  return ret;
}

AnnotationList
AnnotationServerAgent::selectRelatedAnnotationsWithTokenId(qint64 tid)
{
  AnnotationList ret;
  if (tid)
    ret = proxy_->selectRelatedAnnotationsWithTokenId(tid);
  return ret;
}

AliasList
AnnotationServerAgent::selectRelatedAliasesWithTokenId(qint64 tid)
{
  AliasList ret;
  if (tid)
    ret = proxy_->selectRelatedAliasesWithTokenId(tid);
  return ret;
}

AliasList
AnnotationServerAgent::selectAliasesWithTokenId(qint64 tid)
{
  AliasList ret;
  if (tid)
    ret = proxy_->selectAliasesWithTokenId(tid);
  return ret;
}

// - Deletion -

bool
AnnotationServerAgent::deleteAnnotationWithId(qint64 id)
{
  bool ok = false;
  if (id && isAuthorized()) {
    ok = proxy_->deleteAnnotationWithId(id);
    if (!ok && login())
      ok = proxy_->deleteAnnotationWithId(id);
  }
  return ok;
}

bool
AnnotationServerAgent::deleteAliasWithId(qint64 id)
{
  bool ok = false;
  if (id && isAuthorized()) {
    ok = proxy_->deleteAliasWithId(id);
    if (!ok && login())
      ok = proxy_->deleteAliasWithId(id);
  }
  return ok;
}

// - Cast -

#define DO_CAST(_cast, _Cast, _Entity) \
  bool \
  AnnotationServerAgent::_cast##_Entity##WithId(qint64 id) \
  { \
    if (!isAuthorized() || !id) \
      return false; \
 \
    CastEvent e(CastEvent::_Cast##Event, id, Traits::_Entity); \
    return cast_->registerEvent(e) && \
           proxy_->_cast##_Entity##WithId(id); \
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
  AnnotationServerAgent::is##_Entity##_Casted##WithId(qint64 id) const \
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
AnnotationServerAgent::updateGameThread(const GameThread &thread)
{
  if (!isAuthorized())
    return false;
  if (!thread.hasId() || !thread.hasTokenId() || !thread.hasEncoding() || !thread.hasSignature() || !thread.hasProvider())
    return false;
  return proxy_->updateGameThread(thread);
}

bool
AnnotationServerAgent::updateGameHookTextWithId(const QString &text, qint64 id)
{
  if (!isAuthorized())
    return false;
  if (!id)
    return false;

  bool ok = proxy_->updateGameHookTextWithId(text, id);
  if (!ok && login())
    ok = proxy_->updateGameHookTextWithId(text, id);
  return ok;
}

bool
AnnotationServerAgent::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  if (!isAuthorized())
    return false;
  if (!id)
    return false;

  bool ok = proxy_->updateAnnotationTextWithId(text, id);
  if (!ok && login())
    ok = proxy_->updateAnnotationTextWithId(text, id);
  return ok;
}

bool
AnnotationServerAgent::updateAnnotationUserIdWithId(qint64 userId, qint64 id)
{
  if (!id)
    return false;
  if (!isAuthorized() || userId != user_.id())
    return false;
  bool ok = proxy_->updateAnnotationUserIdWithId(userId, id);
  if (!ok && login())
    ok = proxy_->updateAnnotationUserIdWithId(userId, id);
  return ok;
}

// EOF

/*

qint64
AnnotationServerAgent::submitLiveAnnotation(const Annotation &annot)
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

  qint64 ret = proxy_->submitLiveAnnotationTextWithTokenId(annot.text(), Token::TI_Public);
  DOUT("exit: ret =" << ret);
  return ret;
  Q_UNUSED(annot)
  return 0;
}

qint64
AnnotationServerAgent::submitLiveAnnotationText(const QString &text)
{
  if (!isAuthorized()) {
    DOUT("exit: not authorized");
    return 0;
  }
  if (text.isEmpty()) {
    DOUT("exit: missing text");
    return 0;
  }

  qint64 ret = proxy_->submitLiveAnnotationTextWithTokenId(text, Token::TI_Public);
  DOUT("exit: ret =" << ret);
  return ret;
  Q_UNUSED(text)
  return 0;
}


AnnotationList
AnnotationServerAgent::selectLiveAnnotations()
{
  AnnotationList ret = proxy_->selectLiveAnnotationsWithTokenId(Token::TI_Public);
  return ret;
  return AnnotationList();
}

qint32
AnnotationServerAgent::selectLiveTokenInterval()
{
  qint32 ret = proxy_->selectLiveTokenIntervalWithId(Token::TI_Public);
  return ret;
  return 0;
}
*/
