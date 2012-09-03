// annotserverproxy_p.cc
// 9/10/2011
// See: http://blog.csdn.net/tingsking18/article/details/5456831

#include "module/annotservice/annotserverproxy_p.h"
#include "module/annotservice/cloudservicesoap_p.h"
#include "module/annotservice/cloudservice_config.h"
#include "module/annotcloud/traits.h"
#include <QtCore/QMutexLocker>

//#define DEBUG "annotserverproxy"
#include "module/debug/debug.h"

using namespace AnnotCloud;
using namespace CLOUD_SERVICE_NAMESPACE;

enum { SoapConnectionError = 28 };
enum { MaxConnectionRetries = 3 };

// - Construction -

AnnotationServerProxy::AnnotationServerProxy(QObject *parent)
  : Base(parent)
{
  DOUT("enter");
  proxy_ = new CloudServiceProxy(CLOUD_SERVICE_SOAP_MODE);
#ifdef WITH_GZIP
  proxy_->z_level = 6; // compression level, default is 1 (fastest), max is 9
#endif // WIZH_GZIP
#ifdef WITH_COOKIES
  // To avoid "cookie storms" caused by malicious servers that return an unreasonable amount of cookies,
  // gSOAP clients/servers are restricted to a database size that the user can limit (32 cookies by default),
  // See: http://www.cs.fsu.edu/~engelen/soapdoc2.html#tth_sEc19.28
  //
  //proxy_->cookie_max = 10;
#endif //ifdef WITH_COOKIES
  DOUT("exit");
}

AnnotationServerProxy::~AnnotationServerProxy()
{ delete proxy_; }

// - User -

bool
AnnotationServerProxy::setUserAnonymous(bool t)
{
  DOUT("enter: anonymous =" << t);

  tns__setUserAnonymous request;
  request.value = t;

  tns__setUserAnonymousResponse response;
  mutex_.lock();
  int err = proxy_->setUserAnonymous(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->setUserAnonymous(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationServerProxy::setUserLanguage(qint32 language)
{
  DOUT("enter: language =" << language);

  tns__setUserLanguage request;
  request.value = language;

  tns__setUserLanguageResponse response;
  mutex_.lock();
  int err = proxy_->setUserLanguage(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->setUserLanguage(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationServerProxy::isLatestApp(const QString &version)
{
  DOUT("enter: version =" << version);

  tns__isLatestApp request;
  std::string arg0 = version.toStdString();
  request.version = &arg0;

  tns__isLatestAppResponse response;
  mutex_.lock();
  int err = proxy_->isLatestApp(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->isLatestApp(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("exit: ret =" << ret);
  return ret;
}

// - Actions -

/*
bool
AnnotationServerProxy::login(const QString &userName, const QString &password)
{
  DOUT("enter: userName =" << userName);
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
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationServerProxy::logout()
{
  DOUT("enter");
  tns__logout request;
  tns__logoutResponse response;
  mutex_.lock();
  int err = proxy_->logout(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  DOUT("exit: ret =" << true);
  return true;
}
*/

bool
AnnotationServerProxy::getConnected()
{
  DOUT("enter");
  tns__isConnected request;
  tns__isConnectedResponse response;
  mutex_.lock();
  int err = proxy_->isConnected(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->isConnected(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("exit: ret =" << ret);
  return ret;
}

/*
bool
AnnotationServerProxy::getAuthorized()
{
  DOUT("enter");
  tns__isAuthorized request;
  tns__isAuthorizedResponse response;
  mutex_.lock();
  int err = proxy_->isAuthorized(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("exit: ret =" << ret);
  return ret;
}
*/

QString
AnnotationServerProxy::chat(const QString &message)
{
  DOUT("enter");

  std::string arg0 = message.toStdString();

  tns__chat request;
  request.text = &arg0;

  tns__chatResponse response;
  mutex_.lock();
  int err = proxy_->chat(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->chat(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return QString();
  }

  QString ret;
  if (response.return_)
    ret = QString::fromStdString(*response.return_);

  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationServerProxy::login(const QString &userName, const QString &password)
{
  DOUT("enter: userName =" << userName);

  tns__login request;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__loginResponse response;

  mutex_.lock();
  int err = proxy_->login(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->login(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("exit: ret =" << ret);
  return ret;
}

User
AnnotationServerProxy::getUser()
{
  DOUT("enter");
  User ret;

  tns__getUser request;
  tns__getUserResponse response;

  mutex_.lock();
  int err = proxy_->getUser(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->getUser(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  tns__user *p = response.return_;
  if (p && p->id) {
    ret.setStatus(p->status);
    ret.setFlags(p->flags);
    ret.setId(p->id);
    ret.setLanguage(p->language)   ;
    ret.setCreateTime(p->createTime);
    ret.setLoginTime(p->loginTime);
    ret.setLoginIp(p->loginIp);
    ret.setLoginCount(p->loginCount);
    ret.setBlessCount(p->blessCount);
    ret.setCurseCount(p->curseCount);
    ret.setBlockCount(p->blockCount);
    ret.setAnnotCount(p->annotCount);

    if (p->name)
      ret.setName(QString::fromStdString(*p->name));
    if (p->nickname)
      ret.setNickname(QString::fromStdString(*p->nickname));
    if (p->email)
      ret.setEmail(QString::fromStdString(*p->email));
    if (p->password)
      ret.setPassword(QString::fromStdString(*p->password));
  }

  DOUT("exit: userName =" << ret.name() << ", id =" << ret.id());
  return ret;
}

/*
User
AnnotationServerProxy::selectUser(const QString &userName, const QString &password)
{
  DOUT("enter: userName =" << userName);
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
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  tns__user *p = response.return_;
  if (p) {
    ret.setStatus(p->status);
    ret.setFlags(p->flags);
    ret.setId(p->id);
    ret.setLanguage(p->language)   ;
    ret.setCreateTime(p->createTime);
    ret.setLoginTime(p->loginTime);
    ret.setLoginIp(p->loginIp);
    ret.setLoginCount(p->loginCount);
    ret.setBlessCount(p->blessCount);
    ret.setCurseCount(p->curseCount);
    ret.setBlockCount(p->blockCount);
    ret.setAnnotCount(p->annotCount);

    if (p->name)
      ret.setName(QString::fromStdString(*p->name));
    if (p->nickname)
      ret.setNickname(QString::fromStdString(*p->nickname));
    if (p->email)
      ret.setEmail(QString::fromStdString(*p->email));
    //if (p->password)
    //  ret.setPassword(QString::fromStdString(*p->password));
    ret.setPassword(password);
  }

  DOUT("exit: userName =" << ret.name() << ", id =" << ret.id());
  return ret;
}
*/

// - Submissions -

qint64
AnnotationServerProxy::submitGameHook(const GameHook &hook)
{
  DOUT("enter");

  tns__gameHook arg0;
  arg0.flags = hook.flags();
  arg0.id = hook.id();
  arg0.userId = hook.userId();
  arg0.tokenId = hook.tokenId();
  arg0.updateTime = hook.updateTime();
  arg0.updateIp = hook.updateIp();
  std::string text;
  if (hook.hasText()) {
    text = hook.text().toStdString();
    arg0.text = &text;
  }

  tns__submitGameHook request;
  request.hook = &arg0;

  tns__submitGameHookResponse response;
  mutex_.lock();
  int err = proxy_->submitGameHook(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->submitGameHook(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: tid =" << ret);
  return ret;
}

qint64
AnnotationServerProxy::submitGameThread(const GameThread &thread)
{
  DOUT("enter");

  tns__gameThread arg0;
  arg0.flags = thread.flags();
  arg0.id = thread.id();
  arg0.type = thread.type();
  arg0.userId = thread.userId();
  arg0.tokenId = thread.tokenId();
  arg0.updateTime = thread.updateTime();
  arg0.updateIp = thread.updateIp();
  arg0.signature = thread.signature();
  arg0.encoding = thread.encoding();
  std::string provider;
  if (thread.hasProvider()) {
    provider = thread.provider().toStdString();
    arg0.provider = &provider;
  }

  tns__submitGameThread request;
  request.thread = &arg0;

  tns__submitGameThreadResponse response;
  mutex_.lock();
  int err = proxy_->submitGameThread(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->submitGameThread(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: tid =" << ret);
  return ret;
}

qint64
AnnotationServerProxy::submitToken(const Token &token)
{
  DOUT("enter");

  if (token.hasDigest() && token.digest().size() != Traits::TOKEN_DIGEST_LENGTH) {
    DOUT("exit: error: invalid digest =" << token.digest());
    return 0;
  }
  if (token.hasUrl() && token.url().size() != Traits::MAX_URL_LENGTH) {
    DOUT("exit: error: url too long =" << token.url());
    return 0;
  }

  tns__mediaToken arg0;
  arg0.status = token.status();
  arg0.flags = token.flags();
  arg0.id = token.id();
  arg0.type = token.type();
  arg0.userId = token.userId();
  std::string digest;
  if (token.hasDigest()) {
    digest = token.digest().toStdString();
    arg0.digest = &digest;
  }
  std::string url;
  if (token.hasUrl()) {
    url = token.url().toStdString();
    arg0.url = &url;
  }
  arg0.section = token.section();
  arg0.createTime = token.createTime();
  arg0.createIp = 0;
  arg0.blessCount = token.blessCount();
  arg0.curseCount = token.curseCount();
  arg0.blockCount = token.blockCount();
  arg0.visitCount = token.visitCount();
  arg0.annotCount = token.annotCount();

  tns__submitMediaToken request;
  request.token = &arg0;

  tns__submitMediaTokenResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaToken(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->submitMediaToken(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: tid =" << ret);
  return ret;
}

qint64
AnnotationServerProxy::selectTokenIdWithDigest(const QString &digest, qint32 section)
{
  DOUT("enter: section =" << section);
  if (digest.size() != Traits::TOKEN_DIGEST_LENGTH) {
    DOUT("exit: error: invalid digest =" << digest);
    return 0;
  }

  tns__selectMediaTokenIdWithDigest request;
  std::string arg0 = digest.toStdString();
  request.digest = &arg0;
  request.section = section;

  tns__selectMediaTokenIdWithDigestResponse response;
  mutex_.lock();
  int err = proxy_->selectMediaTokenIdWithDigest(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectMediaTokenIdWithDigest(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: tid =" << ret);
  return ret;
}

qint64
AnnotationServerProxy::selectTokenIdWithUrl(const QString &url, qint32 section)
{
  DOUT("enter: section =" << section);
  if (url.isEmpty() || url.size() > Traits::MAX_URL_LENGTH) {
    DOUT("exit: error: invalid url =" << url);
    return 0;
  }

  tns__selectMediaTokenIdWithUrl request;
  std::string arg0 = url.toStdString();
  request.url = &arg0;
  request.section = section;

  tns__selectMediaTokenIdWithUrlResponse response;
  mutex_.lock();
  int err = proxy_->selectMediaTokenIdWithUrl(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectMediaTokenIdWithUrl(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: tid =" << ret);
  return ret;
}

qint64
AnnotationServerProxy::submitTokenDigest(const QString &digest, qint32 section, qint32 tokenType)
{
  DOUT("enter: tokenType =" << tokenType);
  //if (tokenType < 0) {
  //  DOUT("exit: error: invalid tokenType =" << tokenType);
  //  return 0;
  //}
  if (digest.size() != Traits::TOKEN_DIGEST_LENGTH) {
    DOUT("exit: error: invalid digest =" << digest);
    return 0;
  }

  tns__submitMediaTokenDigest request;
  std::string arg0 = digest.toStdString();
  request.digest = &arg0;
  request.section = section;
  request.type = tokenType;

  tns__submitMediaTokenDigestResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaTokenDigest(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->submitMediaTokenDigest(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: tid =" << ret);
  return ret;
}

qint64
AnnotationServerProxy::submitTokenUrl(const QString &url, qint32 section, qint32 tokenType)
{
  DOUT("enter: tokenType =" << tokenType);
  //if (tokenType < 0) {
  //  DOUT("exit: error: invalid tokenType =" << tokenType);
  //  return 0;
  //}
  if (url.isEmpty() || url.size() > Traits::MAX_URL_LENGTH) {
    DOUT("exit: error: invalid url =" << url);
    return 0;
  }

  tns__submitMediaTokenUrl request;
  std::string arg0 = url.toStdString();
  request.url = &arg0;
  request.section = section;
  request.type = tokenType;

  tns__submitMediaTokenUrlResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaTokenUrl(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->submitMediaTokenUrl(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: tid =" << ret);
  return ret;
}

qint64
AnnotationServerProxy::submitAlias(const Alias &alias)
{
  DOUT("enter");

  if (!alias.hasText()) {
    DOUT("exit: error: missing text");
    return 0;
  }
  if (alias.text().size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("exit: error: text too long, size =" << alias.text().size());
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
  arg0.updateIp = 0;
  arg0.blessCount = alias.blessCount();
  arg0.curseCount = alias.curseCount();
  arg0.blockCount = alias.blockCount();

  tns__submitMediaAlias request;
  request.alias = &arg0;

  tns__submitMediaAliasResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAlias(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->submitMediaAlias(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: aid =" << ret);
  return ret;
}

qint64
AnnotationServerProxy::submitAliasTextWithTokenId(const QString &text, qint32 type, qint64 tokenId)
{
  DOUT("enter");

  if (text.isEmpty()) {
    DOUT("exit: error: missing text");
    return 0;
  }
  if (text.size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("exit: error: text too long, size =" << text.size());
    return 0;
  }

  tns__submitMediaAliasTextWithTokenId request;
  std::string arg0 = text.toStdString();
  request.text = &arg0;
  request.type = type;
  request.tokenId = tokenId;

  tns__submitMediaAliasTextWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAliasTextWithTokenId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->submitMediaAliasTextWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: aid =" << ret);
  return ret;
}


qint64
AnnotationServerProxy::submitAliasTextAndTokenDigest(const QString &text, qint32 type, const QString &digest, qint32 section)
{
  DOUT("enter: section =" << section);

  if (text.isEmpty()) {
    DOUT("exit: error: missing text");
    return 0;
  }
  if (text.size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("exit: error: text too long, size =" << text.size());
    return 0;
  }
  if (digest.size() != Traits::TOKEN_DIGEST_LENGTH) {
    DOUT("exit: invalid digest size =" << digest.size());
    return 0;
  }

  tns__submitMediaAliasTextAndTokenDigest request;
  std::string arg0 = text.toStdString();
  std::string arg2 = digest.toStdString();
  request.text = &arg0;
  request.type = type;
  request.tokenDigest = &arg2;
  request.tokenSection = section;

  tns__submitMediaAliasTextAndTokenDigestResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAliasTextAndTokenDigest(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->submitMediaAliasTextAndTokenDigest(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: aid =" << ret);
  return ret;
}

qint64
AnnotationServerProxy::submitAnnotation(const Annotation &annot)
{
  DOUT("enter");

  if (!annot.hasText()) {
    DOUT("exit: error: missing text");
    return 0;
  }
  if (annot.text().size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("exit: error: text too long, size =" << annot.text().size());
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
  arg0.updateIp = 0;
  arg0.blessCount = annot.blessCount();
  arg0.curseCount = annot.curseCount();
  arg0.blockCount = annot.blockCount();

  tns__submitMediaAnnotation request;
  request.annot = &arg0;

  tns__submitMediaAnnotationResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAnnotation(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->submitMediaAnnotation(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: aid =" << ret);
  return ret;
}

qint64
AnnotationServerProxy::submitAnnotationTextWithTokenId(const QString &text, qint64 pos, qint32 posType, qint64 tokenId)
{
  DOUT("enter");

  if (text.isEmpty()) {
    DOUT("exit: error: missing text");
    return 0;
  }
  if (text.size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("exit: error: text too long, size =" << text.size());
    return 0;
  }

  tns__submitMediaAnnotationTextWithTokenId request;
  std::string arg0 = text.toStdString();
  request.text = &arg0;
  request.pos = pos;
  request.posType = posType;
  request.tokenId = tokenId;

  tns__submitMediaAnnotationTextWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAnnotationTextWithTokenId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->submitMediaAnnotationTextWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT(" soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: aid =" << ret);
  return ret;
}

qint64
AnnotationServerProxy::submitAnnotationTextAndTokenDigest(const QString &text, qint64 pos, qint32 posType, const QString &digest, qint32 section)
{
  DOUT("enter: section =" << section);

  if (text.isEmpty()) {
    DOUT("exit: error: missing text");
    return 0;
  }
  if (text.size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("exit: error: text too long, size =" << text.size());
    return 0;
  }
  if (digest.size() != Traits::TOKEN_DIGEST_LENGTH) {
    DOUT("exit: error: text too long, size =" << text.size());
    return 0;
  }

  tns__submitMediaAnnotationTextAndTokenDigest request;
  std::string arg0 = text.toStdString();
  std::string arg3 = digest.toStdString();
  request.text = &arg0;
  request.pos = pos;
  request.posType = posType;
  request.tokenDigest = &arg3;
  request.tokenSection = section;

  tns__submitMediaAnnotationTextAndTokenDigestResponse response;
  mutex_.lock();
  int err = proxy_->submitMediaAnnotationTextAndTokenDigest(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->submitMediaAnnotationTextAndTokenDigest(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: aid =" << ret);
  return ret;
}

// - Queries -

GameHook
AnnotationServerProxy::selectGameHookWithTokenDigest(const QString &digest)
{
  DOUT("enter");
  GameHook ret;

  if (digest.size() != Traits::TOKEN_DIGEST_LENGTH) {
    DOUT("exit: error: invalid digest =" << digest());
    return ret;
  }

  tns__selectGameHookWithTokenDigest request;
  std::string arg0;
  arg0 = digest.toStdString();
  request.tokenDigest = &arg0;

  tns__selectGameHookWithTokenDigestResponse response;
  mutex_.lock();
  int err = proxy_->selectGameHookWithTokenDigest(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectGameHookWithTokenDigest(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  tns__gameHook *p = response.return_;
  if (p) {
    ret.setFlags(p->flags);
    ret.setId(p->id);
    ret.setUserId(p->userId);
    ret.setUpdateTime(p->updateTime);
    ret.setUpdateIp(p->updateIp);
    ret.setTokenId(p->tokenId);
    if (p->text)
      ret.setText(QString::fromStdString(*p->text));
  }
  DOUT("exit: tid =" << ret.id());
  return ret;
}

GameHook
AnnotationServerProxy::selectGameHookWithTokenId(qint64 tid)
{
  DOUT("enter");
  GameHook ret;

  tns__selectGameHookWithTokenId request;
  request.tokenId = tid;

  tns__selectGameHookWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->selectGameHookWithTokenId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectGameHookWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  tns__gameHook *p = response.return_;
  if (p) {
    ret.setFlags(p->flags);
    ret.setTokenId(p->tokenId);
    ret.setId(p->id);
    ret.setUserId(p->userId);
    ret.setUpdateTime(p->updateTime);
    ret.setUpdateIp(p->updateIp);
    if (p->text)
      ret.setText(QString::fromStdString(*p->text));
  }
  DOUT("exit: tid =" << ret.id());
  return ret;
}

GameThread
AnnotationServerProxy::selectGameThreadWithTokenDigest(const QString &digest)
{
  DOUT("enter");
  GameThread ret;

  if (digest.size() != Traits::TOKEN_DIGEST_LENGTH) {
    DOUT("exit: error: invalid digest =" << digest());
    return ret;
  }

  tns__selectGameThreadWithTokenDigest request;
  std::string arg0;
  arg0 = digest.toStdString();
  request.tokenDigest = &arg0;

  tns__selectGameThreadWithTokenDigestResponse response;
  mutex_.lock();
  int err = proxy_->selectGameThreadWithTokenDigest(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectGameThreadWithTokenDigest(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  tns__gameThread *p = response.return_;
  if (p) {
    ret.setFlags(p->flags);
    ret.setId(p->id);
    ret.setType(p->type);
    ret.setUserId(p->userId);
    ret.setUpdateTime(p->updateTime);
    ret.setUpdateIp(p->updateIp);
    ret.setTokenId(p->tokenId);
    ret.setSignature(p->signature);
    ret.setEncoding(p->encoding);
    if (p->provider)
      ret.setProvider(QString::fromStdString(*p->provider));
  }
  DOUT("exit: tid =" << ret.id());
  return ret;
}

GameThread
AnnotationServerProxy::selectGameThreadWithTokenId(qint64 tid)
{
  DOUT("enter");
  GameThread ret;

  tns__selectGameThreadWithTokenId request;
  request.tokenId = tid;

  tns__selectGameThreadWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->selectGameThreadWithTokenId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectGameThreadWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  tns__gameThread *p = response.return_;
  if (p) {
    ret.setFlags(p->flags);
    ret.setTokenId(p->tokenId);
    ret.setId(p->id);
    ret.setType(p->type);
    ret.setUserId(p->userId);
    ret.setUpdateTime(p->updateTime);
    ret.setUpdateIp(p->updateIp);
    ret.setSignature(p->signature);
    ret.setEncoding(p->encoding);
    if (p->provider)
      ret.setProvider(QString::fromStdString(*p->provider));
  }
  DOUT("exit: tid =" << ret.id());
  return ret;
}

Token
AnnotationServerProxy::selectTokenWithId(qint64 id)
{
  DOUT("enter");
  Token ret;

  tns__selectMediaTokenWithId request;
  request.id = id;

  tns__selectMediaTokenWithIdResponse response;
  mutex_.lock();
  int err = proxy_->selectMediaTokenWithId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectMediaTokenWithId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
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
    if (p->url)
      ret.setUrl(QString::fromStdString(*p->url));
    ret.setSection(p->section);
    ret.setCreateTime(p->createTime);
    ret.setCreateIp(p->createIp);
    ret.setBlessCount(p->blessCount);
    ret.setCurseCount(p->curseCount);
    ret.setBlockCount(p->blockCount);
    ret.setVisitCount(p->visitCount);
    ret.setAnnotCount(p->annotCount);
  }
  DOUT("exit: tid =" << ret.id());
  return ret;
}

Token
AnnotationServerProxy::selectTokenWithDigest(const QString &digest, int section)
{
  DOUT("enter: section =" << section);
  Token ret;

  tns__selectMediaTokenWithDigest request;
  std::string arg0 = digest.toStdString();
  request.digest = &arg0;
  request.section = section;

  tns__selectMediaTokenWithDigestResponse response;
  mutex_.lock();
  int err = proxy_->selectMediaTokenWithDigest(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectMediaTokenWithDigest(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
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
    if (p->url)
      ret.setUrl(QString::fromStdString(*p->url));
    ret.setSection(p->section);
    ret.setCreateTime(p->createTime);
    ret.setCreateIp(p->createIp);
    ret.setBlessCount(p->blessCount);
    ret.setCurseCount(p->curseCount);
    ret.setBlockCount(p->blockCount);
    ret.setVisitCount(p->visitCount);
    ret.setAnnotCount(p->annotCount);
  }

  DOUT("exit: tid =" << ret.id());
  return ret;
}

AliasList
AnnotationServerProxy::selectAliasesWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  AliasList ret;

  tns__selectMediaAliasesWithTokenId request;
  request.tokenId = tid;

  tns__selectMediaAliasesWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->selectMediaAliasesWithTokenId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectMediaAliasesWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  if (!response.return_.empty()) {
    Alias a; // improve performance
    foreach (tns__mediaAlias *p, response.return_)
      if (p) {
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
        a.setUpdateIp(p->updateIp);
        a.setBlessCount(p->blessCount);
        a.setCurseCount(p->curseCount);
        a.setBlockCount(p->blockCount);

        ret.append(a);
      }
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

AliasList
AnnotationServerProxy::selectRelatedAliasesWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  AliasList ret;

  tns__selectRelatedMediaAliasesWithTokenId request;
  request.tokenId = tid;

  tns__selectRelatedMediaAliasesWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->selectRelatedMediaAliasesWithTokenId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectRelatedMediaAliasesWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  if (!response.return_.empty()) {
    Alias a; // improve performance
    foreach (tns__mediaAlias *p, response.return_)
      if (p) {
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
        a.setUpdateIp(p->updateIp);
        a.setBlessCount(p->blessCount);
        a.setCurseCount(p->curseCount);
        a.setBlockCount(p->blockCount);

        ret.append(a);
      }
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

AnnotationList
AnnotationServerProxy::selectAnnotationsWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  AnnotationList ret;

  tns__selectMediaAnnotationsWithTokenId request;
  request.tokenId = tid;

  tns__selectMediaAnnotationsWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->selectMediaAnnotationsWithTokenId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectMediaAnnotationsWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  if (!response.return_.empty()) {
    Annotation a; // improve performance
    foreach (tns__mediaAnnotation *p, response.return_)
      if (p) {
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
        a.setUpdateIp(p->updateIp);
        a.setBlessCount(p->blessCount);
        a.setCurseCount(p->curseCount);
        a.setBlockCount(p->blockCount);

        ret.append(a);
      }
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

AnnotationList
AnnotationServerProxy::selectRelatedAnnotationsWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  AnnotationList ret;

  tns__selectRelatedMediaAnnotationsWithTokenId request;
  request.tokenId = tid;

  tns__selectRelatedMediaAnnotationsWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->selectRelatedMediaAnnotationsWithTokenId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->selectRelatedMediaAnnotationsWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  if (!response.return_.empty()) {
    Annotation a; // improve performance
    foreach (tns__mediaAnnotation *p, response.return_)
      if (p) {
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
        a.setUpdateIp(p->updateIp);
        a.setBlessCount(p->blessCount);
        a.setCurseCount(p->curseCount);
        a.setBlockCount(p->blockCount);

        ret.append(a);
      }
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

// - Cast/Delete -

#define CAST_MEDIA(_cast, _entity) \
  bool \
  AnnotationServerProxy::_cast##_entity##WithId(qint64 id) \
  { \
    DOUT("enter: id =" << id); \
 \
    tns__##_cast##Media##_entity##WithId request; \
    request.id = id; \
 \
    tns__##_cast##Media##_entity##WithIdResponse response; \
    mutex_.lock(); \
    int err = proxy_->_cast##Media##_entity##WithId(&request, &response); \
    if (err) \
      for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++) \
        err = proxy_->_cast##Media##_entity##WithId(&request, &response); \
    mutex_.unlock(); \
    if (err) { \
      DOUT("soap error, err =" << err); \
      emit soapError(err); \
      DOUT("exit"); \
      return false; \
    } \
 \
    bool ret = response.return_; \
    DOUT("exit: ret =" << ret); \
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

  CAST_MEDIA(delete, Annotation)
  CAST_MEDIA(delete, Alias)
#undef CAST_MEDIA

#define CAST(_cast, _entity) \
  bool \
  AnnotationServerProxy::_cast##_entity##WithId(qint64 id) \
  { \
    DOUT("enter: id =" << id); \
 \
    tns__##_cast##_entity##WithId request; \
    request.id = id; \
 \
    tns__##_cast##_entity##WithIdResponse response; \
    mutex_.lock(); \
    int err = proxy_->_cast##_entity##WithId(&request, &response); \
    if (err) \
      for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++) \
        err = proxy_->_cast##_entity##WithId(&request, &response); \
    mutex_.unlock(); \
    if (err) { \
      DOUT("soap error, err =" << err); \
      emit soapError(err); \
      DOUT("exit"); \
      return false; \
    } \
 \
    bool ret = response.return_; \
    DOUT("exit: ret =" << ret); \
    return ret; \
  }

  CAST(bless, User)
  CAST(block, User)
  CAST(curse, User)
#undef CAST

// - Update -

bool
AnnotationServerProxy::updateGameThread(const GameThread &thread)
{
  DOUT("enter: id =" << thread.id());

  tns__gameThread arg0;
  arg0.flags = thread.flags();
  arg0.id = thread.id();
  arg0.type = thread.type();
  arg0.userId = thread.userId();
  arg0.tokenId = thread.tokenId();
  arg0.updateTime = thread.updateTime();
  arg0.updateIp = thread.updateIp();
  arg0.signature = thread.signature();
  arg0.encoding = thread.encoding();
  std::string provider;
  if (thread.hasProvider()) {
    provider = thread.provider().toStdString();
    arg0.provider = &provider;
  }

  tns__updateGameThread request;
  request.thread = &arg0;

  tns__updateGameThreadResponse response;
  mutex_.lock();
  int err = proxy_->updateGameThread(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->updateGameThread(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationServerProxy::updateGameHookTextWithId(const QString &text, qint64 id)
{
  DOUT("enter: id =" << id << ", text =" << text);

  tns__updateGameHookTextWithId request;
  std::string arg0 = text.toStdString();
  request.text = &arg0;
  request.id = id;

  tns__updateGameHookTextWithIdResponse response;
  mutex_.lock();
  int err = proxy_->updateGameHookTextWithId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->updateGameHookTextWithId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationServerProxy::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  DOUT("enter: id =" << id << ", text =" << text);

  tns__updateMediaAnnotationTextWithId request;
  std::string arg0 = text.toStdString();
  request.text = &arg0;
  request.id = id;

  tns__updateMediaAnnotationTextWithIdResponse response;
  mutex_.lock();
  int err = proxy_->updateMediaAnnotationTextWithId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->updateMediaAnnotationTextWithId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationServerProxy::updateAnnotationUserIdWithId(qint64 userId, qint64 id)
{
  DOUT("enter: id =" << id << ", userId =" << userId);

  tns__updateMediaAnnotationUserIdWithId request;
  request.userId = userId;
  request.id = id;

  tns__updateMediaAnnotationUserIdWithIdResponse response;
  mutex_.lock();
  int err = proxy_->updateMediaAnnotationUserIdWithId(&request, &response);
  if (err)
    for (int i = 0; err == SoapConnectionError && i < MaxConnectionRetries; i++)
      err = proxy_->updateMediaAnnotationUserIdWithId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("exit: ret =" << ret);
  return ret;
}

// EOF

// - Live -

/*
qint32
AnnotationServerProxy::selectLiveTokenIntervalWithId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  qint32 ret = 0;

  // Revert live token ID
  if (tid < 0)
    tid = - tid;

  tns__selectLiveTokenWithId request;
  request.arg0 = tid;

  tns__selectLiveTokenWithIdResponse response;
  mutex_.lock();
  int err = proxy_->selectLiveTokenWithId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  tns__liveToken *p = response.return_;
  if (p && p->id)
    ret = p->interval;

  DOUT("exit: ret =" << ret);
  return ret;
}

AnnotationList
AnnotationServerProxy::selectLiveAnnotationsWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  AnnotationList ret;

  // Revert live token ID
  if (tid < 0)
    tid = - tid;

  tns__selectLiveAnnotationsWithTokenId request;
  request.arg0 = tid;

  tns__selectLiveAnnotationsWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->selectLiveAnnotationsWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT("soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return ret;
  }

  if (!response.return_.empty())
    foreach (tns__liveAnnotation *p, response.return_)
      if (p) {
        Annotation a;
        a.setStatus(p->status);
        a.setFlags(p->flags);
        a.setId(p->id);
        a.setTokenId(p->tokenId > 0 ? - p->tokenId : p->tokenId); // Revert
        a.setUserId(p->userId);
        if (p->userAlias)
          a.setUserAlias(QString::fromStdString(*p->userAlias));
        a.setLanguage(p->language);
        if (p->text)
          a.setText(QString::fromStdString(*p->text));
        a.setCreateTime(p->createTime);

        ret.append(a);
      }

  DOUT("exit: count =" << ret.size());
  return ret;
}

qint64
AnnotationServerProxy::submitLiveAnnotationTextWithTokenId(const QString &text, qint64 tokenId, const QString &userName, const QString &password)
{
  DOUT("enter");

  if (text.isEmpty()) {
    DOUT("exit: error: missing text");
    return 0;
  }
  if (text.size() > Traits::MAX_ANNOT_LENGTH) {
    DOUT("exit: error: text too long, size =" << text.size());
    return 0;
  }

  // Revert live token ID
  if (tokenId < 0)
    tokenId = - tokenId;

  tns__submitLiveAnnotationTextWithTokenId request;
  std::string arg0 = text.toStdString();
  request.arg0 = &arg0;
  request.arg1 = tokenId;
  std::string s_userName = userName.toStdString();
  request.userName = &s_userName;
  std::string s_password = password.toStdString();
  request.password = &s_password;

  tns__submitLiveAnnotationTextWithTokenIdResponse response;
  mutex_.lock();
  int err = proxy_->submitLiveAnnotationTextWithTokenId(&request, &response);
  mutex_.unlock();
  if (err) {
    DOUT(" soap error, err =" << err);
    emit soapError(err);
    DOUT("exit");
    return 0;
  }

  qint64 ret = response.return_;
  DOUT("exit: aid =" << ret);
  return ret;
}
*/
