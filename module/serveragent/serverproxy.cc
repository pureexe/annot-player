// module/serveragent/serverproxy.cc
// 9/10/2011
// See: http://blog.csdn.net/tingsking18/article/details/5456831

#include "serverproxy.h"
#include "serverproxy_config.h"
#include "core/cloud/traits.h"
#include <QtCore>

#define DEBUG "ServerProxy"
#include "module/debug/debug.h"

using namespace Core::Cloud;
using namespace ServerSoap;

// - Construction -

ServerProxy::ServerProxy(QObject *parent)
  : Base(parent)
{ reset(); }

void
ServerProxy::reset()
{
  DOUT("reset:enter");
  mutex_.lock();
  proxy_.reset(
    new ServerSoapProxy(SERVER_SOAP_MODE)
  );
#ifdef WITH_GZIP
  proxy_->z_level = 6; // compression level, default is 1 (fastest), max is 9
#endif // WIZH_GZIP
  mutex_.unlock();
  DOUT("reset:exit");
}

// - User -

bool
ServerProxy::setUserAnonymous(bool t, const QString &userName, const QString &password)
{
  DOUT("setUserAnonymous:enter: anonymous =" << t);

  tns__setUserAnonymous request;
  request.arg0 = t;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.userName = &s_password;

  tns__setUserAnonymousResponse response;
  mutex_.lock();
  int err = proxy_->setUserAnonymous(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("setUserAnonymous: soap error, err =" << err);
    emit soapError(err);
    DOUT("setUserAnonymous:exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("setUserAnonymous:exit: ret =" << ret);
  return ret;
}

bool
ServerProxy::setUserLanguage(qint32 language, const QString &userName, const QString &password)
{
  DOUT("setUserLanguage:enter: language =" << language);

  tns__setUserLanguage request;
  request.arg0 = language;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__setUserLanguageResponse response;
  mutex_.lock();
  int err = proxy_->setUserLanguage(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("setUserLanguage: soap error, err =" << err);
    emit soapError(err);
    DOUT("setUserLanguage:exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("setUserLanguage:exit: ret =" << ret);
  return ret;
}

bool
ServerProxy::isClientUpdated(const QString &version)
{
  DOUT("isClientUpdated:enter: version =" << version);

  tns__isClientUpdated request;
  std::string arg0 = version.toStdString();
  request.arg0 = &arg0;

  tns__isClientUpdatedResponse response;
  mutex_.lock();
  int err = proxy_->isClientUpdated(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("isClientUpdated: soap error, err =" << err);
    emit soapError(err);
    DOUT("isClientUpdated:exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("isClientUpdated:exit: ret =" << ret);
  return ret;
}

// - Actions -

/*
bool
ServerProxy::login(const QString &userName, const QString &password)
{
  DOUT("login:enter: userName =" << userName);
  std::string arg0 = userName.toStdString(),
              arg1 = password.toStdString();

  tns__login request;
  request.arg0 = &arg0;
  request.arg1 = &arg1;

  tns__loginResponse response;
  mutex_.lock();
  int err = proxy_->login(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("login: soap error, err =" << err);
    emit soapError(err);
    DOUT("login:exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("login:exit: ret =" << ret);
  return ret;
}

bool
ServerProxy::logout()
{
  DOUT("logout:enter");
  tns__logout request;
  tns__logoutResponse response;
  mutex_.lock();
  int err = proxy_->logout(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("logout: soap error, err =" << err);
    emit soapError(err);
    DOUT("logout:exit");
    return false;
  }

  DOUT("logout:exit: ret =" << true);
  return true;
}
*/

bool
ServerProxy::getConnected()
{
  DOUT("getConnected:enter");
  tns__isConnected request;
  tns__isConnectedResponse response;
  mutex_.lock();
  int err = proxy_->isConnected(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("getConnected: soap error, err =" << err);
    emit soapError(err);
    DOUT("getConnected:exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("getConnected:exit: ret =" << ret);
  return ret;
}

/*
bool
ServerProxy::getAuthorized()
{
  DOUT("getAuthorized:enter");
  tns__isAuthorized request;
  tns__isAuthorizedResponse response;
  mutex_.lock();
  int err = proxy_->isAuthorized(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("getAuthorized: soap error, err =" << err);
    emit soapError(err);
    DOUT("getAuthorized:exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("getAuthorized:exit: ret =" << ret);
  return ret;
}
*/

QString
ServerProxy::chat(const QString &message)
{
  DOUT("chat:enter");

  std::string arg0 = message.toStdString();

  tns__chat request;
  request.arg0 = &arg0;

  tns__chatResponse response;
  mutex_.lock();
  int err = proxy_->chat(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("chat: soap error, err =" << err);
    emit soapError(err);
    DOUT("chat:exit");
    return QString();
  }

  QString ret;
  if (response.return_)
    ret = QString::fromStdString(*response.return_);

  DOUT("chat:exit: ret =" << ret);
  return ret;
}

User
ServerProxy::selectUser(const QString &userName, const QString &password)
{
  DOUT("selectUser:enter: userName =" << userName);
  User ret;

  tns__selectUser request;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__selectUserResponse response;

  mutex_.lock();
  int err = proxy_->selectUser(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("selectUser: soap error, err =" << err);
    emit soapError(err);
    DOUT("selectUser:exit");
    return ret;
  }

  tns__user *p = response.return_;
  if (p) {
    ret.setStatus(p->status);
    ret.setFlags(p->flags);
    ret.setId(p->id);
    ret.setGroupId(p->groupId);
    ret.setLanguage(p->language)   ;
    ret.setCreateTime(p->createTime);
    ret.setLoginTime(p->loginTime);
    ret.setBlessedCount(p->blessedCount);
    ret.setCursedCount(p->cursedCount);
    ret.setBlockedCount(p->blockedCount);
    ret.setAnnotCount(p->annotCount);

    if (p->name)
      ret.setName(QString::fromStdString(*p->name));
    if (p->nickname)
      ret.setNickname(QString::fromStdString(*p->nickname));
    //if (p->password)
    //  ret.setPassword(QString::fromStdString(*p->password));
    ret.setPassword(password);
  }

  DOUT("selectUser:exit: username =" << ret.name() << ", id =" << ret.id());
  return ret;
}

// - Submissions -

qint64
ServerProxy::submitToken(const Token &token, const QString &userName, const QString &password)
{
  DOUT("submitToken:enter");

  if (!token.hasDigest() || token.digest().length() != Traits::TOKEN_DIGEST_LENGTH) {
    DOUT("submitToken:exit: error: invalid digest =" << token.digest());
    return 0;
  }

  tns__mediaToken arg0;
  arg0.status = token.status();
  arg0.flags = token.flags();
  arg0.id = token.id();
  arg0.type = token.type();
  arg0.userId = token.userId();
  std::string digest = token.digest().toStdString();
  arg0.digest = &digest;
  arg0.digestType = token.digestType();
  arg0.createTime = token.createTime();
  arg0.blessedCount = token.blessedCount();
  arg0.cursedCount = token.cursedCount();
  arg0.blockedCount = token.blockedCount();
  arg0.visitedCount = token.visitedCount();
  arg0.annotCount = token.annotCount();

  tns__submitMediaToken request;
  request.arg0 = &arg0;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__submitMediaTokenResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaToken(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("submitToken: soap error, err =" << err);
    emit soapError(err);
    DOUT("submitToken:exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("submitToken:exit: tid =" << ret);
  return ret;
}

qint64
ServerProxy::submitTokenDigest(const QString &digest, qint32 digestType, qint32 tokenType, const QString &userName, const QString &password)
{
  DOUT("submitTokenDigest:enter: tokenType =" << tokenType);
  //if (tokenType < 0) {
  //  DOUT("submitTokenDigest:exit: error: invalid tokenType =" << tokenType);
  //  return 0;
  //}
  if (digest.length() != Traits::TOKEN_DIGEST_LENGTH) {
    DOUT("submitTokenDigest:exit: error: invalid digest =" << digest);
    return 0;
  }

  tns__submitMediaTokenDigest request;
  std::string arg0 = digest.toStdString();
  request.arg0 = &arg0;
  request.arg1 = digestType;
  request.arg2 = tokenType;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__submitMediaTokenDigestResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaTokenDigest(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("submitTokenDigest: soap error, err =" << err);
    emit soapError(err);
    DOUT("submitTokenDigest:exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("submitToken:exit: tid =" << ret);
  return ret;
}

qint64
ServerProxy::submitAlias(const Alias &alias, const QString &userName, const QString &password)
{
  DOUT("submitAlias:enter");

  if (!alias.hasText()) {
    DOUT("submitAlias:exit: error: missing text");
    return 0;
  }
  if (alias.text().size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("submitAlias:exit: error: text too long, size =" << alias.text().size());
    return 0;
  }

  tns__mediaAlias arg0;
  arg0.status = alias.status();
  arg0.flags = alias.flags();
  arg0.id = alias.id();
  arg0.tokenId = alias.tokenId();
  arg0.userId = alias.userId();
  arg0.type = alias.type();
  std::string text = alias.text().toStdString();
  arg0.text = &text;
  arg0.language = alias.language();
  arg0.updateTime = alias.updateTime();
  arg0.blessedCount = alias.blessedCount();
  arg0.cursedCount = alias.cursedCount();
  arg0.blockedCount = alias.blockedCount();

  tns__submitMediaAlias request;
  request.arg0 = &arg0;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__submitMediaAliasResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAlias(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("submitAlias: soap error, err =" << err);
    emit soapError(err);
    DOUT("submitAlias:exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("submitAlias:exit: aid =" << ret);
  return ret;
}

qint64
ServerProxy::submitAliasTextWithTokenId(const QString &text, qint32 type, qint64 tokenId, const QString &userName, const QString &password)
{
  DOUT("submitAliasTextWithTokenId:enter");

  if (text.isEmpty()) {
    DOUT("submitAliasTextWithTokenId:exit: error: missing text");
    return 0;
  }
  if (text.size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("submitAliasTextWithTokenId:exit: error: text too long, size =" << text.size());
    return 0;
  }

  tns__submitMediaAliasTextWithTokenId request;
  std::string arg0 = text.toStdString();
  request.arg0 = &arg0;
  request.arg1 = type;
  request.arg2 = tokenId;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__submitMediaAliasTextWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAliasTextWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("submitAliasTextWithTokenId: soap error, err =" << err);
    emit soapError(err);
    DOUT("submitAliasTextWithTokenId:exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("submitAliasTextWithTokenId:exit: aid =" << ret);
  return ret;
}


qint64
ServerProxy::submitAliasTextAndTokenDigest(const QString &text, qint32 type, const QString &digest, qint32 digestType, const QString &userName, const QString &password)
{
  DOUT("submitAliasTextAndTokenDigest:enter: digestType =" << digestType);

  if (text.isEmpty()) {
    DOUT("submitAliasTextAndTokenDigest:exit: error: missing text");
    return 0;
  }
  if (text.size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("submitAliasTextAndTokenDigest:exit: error: text too long, size =" << text.size());
    return 0;
  }
  if (digest.size() != Traits::TOKEN_DIGEST_LENGTH) {
    DOUT("submitAliasTextAndTokenDigest:exit: invalid digest size =" << digest.size());
    return 0;
  }

  tns__submitMediaAliasTextAndTokenDigest request;
  std::string arg0 = text.toStdString();
  std::string arg2 = digest.toStdString();
  request.arg0 = &arg0;
  request.arg1 = type;
  request.arg2 = &arg2;
  request.arg3 = digestType;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__submitMediaAliasTextAndTokenDigestResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAliasTextAndTokenDigest(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("submitAliasTextAndTokenDigest: soap error, err =" << err);
    emit soapError(err);
    DOUT("submitAliasTextAndTokenDigest:exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("submitAliasTextAndTokenDigest:exit: aid =" << ret);
  return ret;
}

qint64
ServerProxy::submitAnnotation(const Annotation &annot, const QString &userName, const QString &password)
{
  DOUT("submitAnnotation:enter");

  if (!annot.hasText()) {
    DOUT("submitAnnotation:exit: error: missing text");
    return 0;
  }
  if (annot.text().size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("submitAnnotation:exit: error: text too long, size =" << annot.text().size());
    return 0;
  }

  tns__mediaAnnotation arg0;
  arg0.status = annot.status();
  arg0.flags = annot.flags();
  arg0.id = annot.id();
  arg0.tokenId = annot.tokenId();
  arg0.userId = annot.userId();
  std::string userAlias = annot.userAlias().toStdString();
  arg0.userAlias = &userAlias;
  arg0.pos = annot.pos();
  arg0.posType = annot.posType();
  arg0.time = annot.time();
  std::string text = annot.text().toStdString();
  arg0.text = &text;
  arg0.language = annot.language();
  arg0.createTime = annot.createTime();
  arg0.updateTime = annot.updateTime();
  arg0.blessedCount = annot.blessedCount();
  arg0.cursedCount = annot.cursedCount();
  arg0.blockedCount = annot.blockedCount();

  tns__submitMediaAnnotation request;
  request.arg0 = &arg0;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__submitMediaAnnotationResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAnnotation(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("submitAnnotation: soap error, err =" << err);
    emit soapError(err);
    DOUT("submitAnnotation:exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("submitAnnotation:exit: aid =" << ret);
  return ret;
}

qint64
ServerProxy::submitAnnotationTextWithTokenId(const QString &text, qint64 pos, qint32 posType, qint64 tokenId, const QString &userName, const QString &password)
{
  DOUT("submitAnnotationTextWithTokenId:enter");

  if (text.isEmpty()) {
    DOUT("submitAnnotationTextWithTokenId:exit: error: missing text");
    return 0;
  }
  if (text.size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("submitAnnotationTextWithTokenId:exit: error: text too long, size =" << text.size());
    return 0;
  }

  tns__submitMediaAnnotationTextWithTokenId request;
  std::string arg0 = text.toStdString();
  request.arg0 = &arg0;
  request.arg1 = pos;
  request.arg2 = posType;
  request.arg3 = tokenId;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__submitMediaAnnotationTextWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAnnotationTextWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("submitAnnotationTextWithTokenId: soap error, err =" << err);
    emit soapError(err);
    DOUT("submitAnnotationTextWithTokenId:exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("submitAnnotationTextWithTokenId:exit: aid =" << ret);
  return ret;
}

qint64
ServerProxy::submitAnnotationTextAndTokenDigest(const QString &text, qint64 pos, qint32 posType, const QString &digest, qint32 digestType, const QString &userName, const QString &password)
{
  DOUT("submitAnnotationTextAndTokenDigest:enter: digestType =" << digestType);

  if (text.isEmpty()) {
    DOUT("submitAnnotationTextAndTokenDigest:exit: error: missing text");
    return 0;
  }
  if (text.size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("submitAnnotationTextAndTokenDigest:exit: error: text too long, size =" << text.size());
    return 0;
  }
  if (digest.size() != Traits::TOKEN_DIGEST_LENGTH) {
    DOUT("submitAnnotationTextAndTokenDigest:exit: error: text too long, size =" << text.size());
    return 0;
  }

  tns__submitMediaAnnotationTextAndTokenDigest request;
  std::string arg0 = text.toStdString();
  std::string arg3 = digest.toStdString();
  request.arg0 = &arg0;
  request.arg1 = pos;
  request.arg2 = posType;
  request.arg3 = &arg3;
  request.arg4 = digestType;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__submitMediaAnnotationTextAndTokenDigestResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAnnotationTextAndTokenDigest(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("submitAnnotationTextAndTokenDigest: soap error, err =" << err);
    emit soapError(err);
    DOUT("submitAnnotationTextAndTokenDigest:exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("submitAnnotationTextAndTokenDigest:exit: aid =" << ret);
  return ret;
}

// - Queries -

Token
ServerProxy::selectTokenWithId(qint64 id)
{
  DOUT("selectTokenWithId:enter");
  Token ret;

  tns__selectMediaTokenWithId request;
  request.arg0 = id;

  tns__selectMediaTokenWithIdResponse response;
  mutex_.lock();
  int err = proxy_->selectMediaTokenWithId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("selectTokenWithId: soap error, err =" << err);
    emit soapError(err);
    DOUT("selectTokenWithId:exit");
    return ret;
  }

  tns__mediaToken *p = response.return_;
  if (p) {
    ret.setStatus(p->status);
    ret.setFlags(p->flags);
    ret.setId(p->id);
    ret.setType(p->type);
    ret.setUserId(p->userId);
    if (p->digest)
      ret.setDigest(QString::fromStdString(*p->digest));
    ret.setDigestType(p->digestType);
    ret.setCreateTime(p->createTime);
    ret.setBlessedCount(p->blessedCount);
    ret.setCursedCount(p->cursedCount);
    ret.setBlockedCount(p->blockedCount);
    ret.setVisitedCount(p->visitedCount);
    ret.setAnnotCount(p->annotCount);
  }
  DOUT("selectTokenWithId:exit: tid =" << ret.id());
  return ret;
}

Token
ServerProxy::selectTokenWithDigest(const QString &digest, int digestType)
{
  DOUT("selectTokenWithDigest:enter: digestType =" << digestType);
  Token ret;

  tns__selectMediaTokenWithDigest request;
  std::string arg0 = digest.toStdString();
  request.arg0 = &arg0;
  request.arg1 = digestType;

  tns__selectMediaTokenWithDigestResponse response;
  mutex_.lock();
  int err = proxy_->selectMediaTokenWithDigest(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("selectTokenWithDigest: soap error, err =" << err);
    emit soapError(err);
    DOUT("selectTokenWithDigest:exit");
    return ret;
  }

  tns__mediaToken *p = response.return_;
  if (p) {
    ret.setStatus(p->status);
    ret.setFlags(p->flags);
    ret.setId(p->id);
    ret.setType(p->type);
    ret.setUserId(p->userId);
    ret.setDigest(digest);
    ret.setDigestType(p->digestType);
    ret.setCreateTime(p->createTime);
    ret.setBlessedCount(p->blessedCount);
    ret.setCursedCount(p->cursedCount);
    ret.setBlockedCount(p->blockedCount);
    ret.setVisitedCount(p->visitedCount);
    ret.setAnnotCount(p->annotCount);
  }

  DOUT("selectTokenWithId:exit: tid =" << ret.id());
  return ret;
}

AliasList
ServerProxy::selectAliasesWithTokenId(qint64 tid)
{
  DOUT("selectAliasesWithTokenId:enter: tid =" << tid);
  AliasList ret;

  tns__selectMediaAliasesWithTokenId request;
  request.arg0 = tid;

  tns__selectMediaAliasesWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->selectMediaAliasesWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("selectAliasesWithTokenId: soap error, err =" << err);
    emit soapError(err);
    DOUT("selectAliasesWithTokenId:exit");
    return ret;
  }

  if (!response.return_.empty())
    foreach (tns__mediaAlias *p, response.return_)
      if (p) {
        Alias a;
        a.setStatus(p->status);
        a.setFlags(p->flags);
        a.setId(p->id);
        a.setTokenId(p->tokenId);
        a.setUserId(p->userId);
        a.setType(p->type);
        a.setLanguage(p->language);
        if (p->text)
          a.setText(QString::fromStdString(*p->text));
        a.setUpdateTime(p->updateTime);
        a.setBlessedCount(p->blessedCount);
        a.setCursedCount(p->cursedCount);
        a.setBlockedCount(p->blockedCount);

        ret.append(a);
      }

  DOUT("selectAliasesWithTokenId:exit: count =" << ret.size());
  return ret;
}

AnnotationList
ServerProxy::selectAnnotationsWithTokenId(qint64 tid)
{
  DOUT("selectAnnotationsWithTokenId:enter: tid =" << tid);
  AnnotationList ret;

  tns__selectMediaAnnotationsWithTokenId request;
  request.arg0 = tid;

  tns__selectMediaAnnotationsWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->selectMediaAnnotationsWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("selectAnnotationsWithTokenId: soap error, err =" << err);
    emit soapError(err);
    DOUT("selectAnnotationsWithTokenId:exit");
    return ret;
  }

  if (!response.return_.empty())
    foreach (tns__mediaAnnotation *p, response.return_)
      if (p) {
        Annotation a;
        a.setStatus(p->status);
        a.setFlags(p->flags);
        a.setId(p->id);
        a.setTokenId(p->tokenId);
        a.setUserId(p->userId);
        if (p->userAlias)
          a.setUserAlias(QString::fromStdString(*p->userAlias));
        a.setPos(p->pos);
        a.setPosType(p->posType);
        a.setTime(p->time);
        a.setLanguage(p->language);
        if (p->text)
          a.setText(QString::fromStdString(*p->text));
        a.setCreateTime(p->createTime);
        a.setUpdateTime(p->updateTime);
        a.setBlessedCount(p->blessedCount);
        a.setCursedCount(p->cursedCount);
        a.setBlockedCount(p->blockedCount);

        ret.append(a);
      }

  DOUT("selectAnnotationsWithTokenId:exit: count =" << ret.size());
  return ret;
}

AnnotationList
ServerProxy::selectRelatedAnnotationsWithTokenId(qint64 tid)
{
  DOUT("selectRelatedAnnotationsWithTokenId:enter: tid =" << tid);
  AnnotationList ret;

  tns__selectRelatedMediaAnnotationsWithTokenId request;
  request.arg0 = tid;

  tns__selectRelatedMediaAnnotationsWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->selectRelatedMediaAnnotationsWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("selectRelatedAnnotationsWithTokenId: soap error, err =" << err);
    emit soapError(err);
    DOUT("selectRelatedAnnotationsWithTokenId:exit");
    return ret;
  }

  if (!response.return_.empty())
    foreach (tns__mediaAnnotation *p, response.return_)
      if (p) {
        Annotation a;
        a.setStatus(p->status);
        a.setFlags(p->flags);
        a.setId(p->id);
        a.setTokenId(p->tokenId);
        a.setUserId(p->userId);
        if (p->userAlias)
          a.setUserAlias(QString::fromStdString(*p->userAlias));
        a.setPos(p->pos);
        a.setPosType(p->posType);
        a.setTime(p->time);
        a.setLanguage(p->language);
        if (p->text)
          a.setText(QString::fromStdString(*p->text));
        a.setCreateTime(p->createTime);
        a.setUpdateTime(p->updateTime);
        a.setBlessedCount(p->blessedCount);
        a.setCursedCount(p->cursedCount);
        a.setBlockedCount(p->blockedCount);

        ret.append(a);
      }

  DOUT("selectRelatedAnnotationsWithTokenId:exit: count =" << ret.size());
  return ret;
}

// - Cast -

#define CAST_MEDIA(_cast, _entity) \
  bool \
  ServerProxy::_cast##_entity##WithId(qint64 id, const QString &userName, const QString &password) \
  { \
    DOUT(#_cast #_entity "WithId:enter: id =" << id); \
 \
    tns__##_cast##Media##_entity##WithId request; \
    request.arg0 = id; \
    std::string s_userName = userName.toStdString(); \
    request.userName = &s_userName; \
    std::string s_password = password.toStdString(); \
    request.password = &s_password; \
 \
    tns__##_cast##Media##_entity##WithIdResponse response; \
    mutex_.lock(); \
    int err = proxy_->_cast##Media##_entity##WithId(&request, &response); \
    mutex_.unlock(); \
    if (err) { \
      DOUT(#_cast #_entity "WithId: soap error, err =" << err); \
      emit soapError(err); \
      DOUT(#_cast #_entity "WithId:exit"); \
      return false; \
    } \
 \
    bool ret = response.return_; \
    DOUT(#_cast #_entity "WithId:exit: ret =" << ret); \
    return ret; \
  }

  CAST_MEDIA(bless, Annotation)
  CAST_MEDIA(curse, Annotation)
  CAST_MEDIA(block, Annotation)

  CAST_MEDIA(bless, Alias)
  CAST_MEDIA(curse, Alias)
  CAST_MEDIA(block, Alias)

  CAST_MEDIA(bless, Token)
  CAST_MEDIA(curse, Token)
#undef CAST_MEDIA

#define CAST(_cast, _entity) \
  bool \
  ServerProxy::_cast##_entity##WithId(qint64 id, const QString &userName, const QString &password) \
  { \
    DOUT(#_cast #_entity "WithId:enter: id =" << id); \
 \
    tns__##_cast##_entity##WithId request; \
    request.arg0 = id; \
    std::string s_userName = userName.toStdString(); \
    request.userName = &s_userName; \
    std::string s_password = password.toStdString(); \
    request.password = &s_password; \
 \
    tns__##_cast##_entity##WithIdResponse response; \
    mutex_.lock(); \
    int err = proxy_->_cast##_entity##WithId(&request, &response); \
    mutex_.unlock(); \
    if (err) { \
      DOUT(#_cast #_entity "WithId: soap error, err =" << err); \
      emit soapError(err); \
      DOUT(#_cast #_entity "WithId:exit"); \
      return false; \
    } \
 \
    bool ret = response.return_; \
    DOUT(#_cast #_entity "WithId:exit: ret =" << ret); \
    return ret; \
  }

  CAST(block, User)
#undef CAST

// - Update -

bool
ServerProxy::updateAnnotationTextWithId(const QString &text, qint64 id, const QString &userName, const QString &password)
{
  DOUT("updateAnnotationTextWithId:enter: id =" << id << ", text =" << text);

  tns__updateMediaAnnotationTextWithId request;
  std::string arg0 = text.toStdString();
  request.arg0 = &arg0;
  request.arg1 = id;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__updateMediaAnnotationTextWithIdResponse response;
  mutex_.lock();
  int err = proxy_->updateMediaAnnotationTextWithId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("updateAnnotationTextWithId: soap error, err =" << err);
    emit soapError(err);
    DOUT("updateAnnotationTextWithId:exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("updateAnnotationTextWithId:exit: ret =" << ret);
  return ret;
}

// EOF
