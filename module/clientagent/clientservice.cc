// module/clientagent/clientservice.cc
// 10/7/2011
// See: http://www.cs.fsu.edu/~engelen/soapdoc2.html

#include "clientservice.h"
#include "clientservice_config.h"

#include "core/universe/traits.h"
#include <QtCore>

#define DEBUG "ClientService"
#include "module/debug/debug.h"

using namespace Core::Universe;
using namespace ClientSoap;

// - Servant -

ClientServant::ClientServant(QObject *parent)
  : QtBase(parent),
    SoapBase(CLIENT_SOAP_MODE),
    delegate_(0), port_(CLIENT_SERVICE_PORT)
{
#ifdef WITH_GZIP
  soap::z_level = 9; // compression level, default is 1 (fastest), max is 9
#endif // WIZH_GZIP
}

ClientDelegate*
ClientServant::delegate() const
{ return delegate_; }

void
ClientServant::setDelegate(ClientDelegate *delegate)
{ delegate_ = delegate; }

int
ClientServant::port() const
{ return port_; }

void
ClientServant::setPort(int port)
{ port_ = port; }

void
ClientServant::run()
{
  DOUT("ClientServant:run:enter");
  int err = SoapBase::run(port_);
  Q_UNUSED(err);
  DOUT("ClientServant:run:exit, errcode =" << err);
}

int
ClientServant::authorize(tns__authorize *request, tns__authorizeResponse *response)
{
  DOUT("ClientServant:authorize:enter");
  if (delegate_)
    response->return_ = delegate_->authorize(request->arg0);
  DOUT("ClientServant:authorize:exit");
  return SOAP_OK;
}

int
ClientServant::deauthorize(tns__deauthorize *request, tns__deauthorizeResponse *response)
{
  Q_UNUSED(request);
  Q_UNUSED(response);
  DOUT("ClientServant:deauthorize:enter");
  if (delegate_)
    delegate_->deauthorize();
  DOUT("ClientServant:deauthorize:exit");
  return SOAP_OK;
}

int
ClientServant::isAuthorized(tns__isAuthorized *request, tns__isAuthorizedResponse *response)
{
  Q_UNUSED(request);
  DOUT("ClientServant:isAuthorize:enter");
  if (delegate_)
    response->return_ = delegate_->isAuthorized();
  DOUT("ClientServant:isAuthorize:exit");
  return SOAP_OK;
}

int
ClientServant::isConnected(tns__isConnected *request, tns__isConnectedResponse *response)
{
  Q_UNUSED(request);
  DOUT("ClientServant:isConnected:enter");
  if (delegate_)
    response->return_ = delegate_->isConnected();
  DOUT("ClientServant:isConnected:exit");
  return SOAP_OK;
}

int
ClientServant::chat(tns__chat *request, tns__chatResponse *response)
{
  Q_UNUSED(response);
  DOUT("ClientServant:chat:enter");
  if (delegate_) {
    if (request->arg0)
      delegate_->chat(QString::fromStdString(*request->arg0));
    else
      delegate_->chat(QString());
  }
  DOUT("ClientServant:chat:exit");
  return SOAP_OK;
}

// - Service -

// ++ Construction ++

ClientService::ClientService(QObject *parent)
  : Base(parent), servant_(0), delegate_(0), port_(CLIENT_SERVICE_PORT)
{ }

ClientService::~ClientService()
{
  DOUT("~ClientService:enter");
  if (isRunning()) {
    DOUT("~ClientService: try stopping running service");
    stop();
  }
  DOUT("~ClientService:exit");
}

// ++ Properties ++

ClientDelegate*
ClientService::delegate() const
{ return delegate_; }

void
ClientService::setDelegate(ClientDelegate *delegate)
{ delegate_ = delegate; }

int
ClientService::port() const
{ return port_; }

void
ClientService::setPort(int port)
{
  DOUT("setPort: old port_ =" << port_ << ", new port =" << port);
  port_ = port;
}

// ++ Service control ++

bool
ClientService::isRunning() const
{ return servant_ && servant_->isRunning(); }

void
ClientService::start()
{
  DOUT("start:enter");
  if (isRunning()) {
    DOUT("start:exit: WARNING: service is already running");
    return;
  }

  DOUT("start: try starting service, port =" << port_);
  servant_ = new ClientServant(this);
  servant_->setDelegate(delegate_);
  servant_->setPort(port_);
  servant_->start();
  DOUT("start:exit");
}

void
ClientService::stop(long msecs)
{
  DOUT("stop:enter");
  if (isRunning()) {
    DOUT("stop: try stopping running service");
    servant_->terminate();
    servant_->wait(msecs);
  } else
    DOUT("stop: WARNING: service is not running");

  DOUT("stop:exit");
}

void
ClientService::restart()
{
  DOUT("restart:enter");
  if (isRunning())
    stop();
  if (!isRunning())
    start();
  DOUT("restart:exit");
}

/*
// - Actions -

bool
ClientService::login(const QString &userName, const QString &password)
{
  DOUT("login:enter: userName =" << userName);
  std::string arg0 = userName.toStdString(),
              arg1 = password.toStdString();

  ns1__login request;
  request.arg0 = &arg0;
  request.arg1 = &arg1;

  ns1__loginResponse response;
  int err = soap_.__ns1__login(&request, &response);
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
ClientService::logout()
{
  DOUT("logout:enter");
  ns1__logout request;
  ns1__logoutResponse response;
  int err = soap_.__ns1__logout(&request, &response);
  if (err) {
    DOUT("logout: soap error, err =" << err);
    emit soapError(err);
    DOUT("logout:exit");
    return false;
  }

  DOUT("logout:exit: ret =" << true);
  return true;
}

bool
ClientService::getConnected()
{
  DOUT("getConnected:enter");
  ns1__isConnected request;
  ns1__isConnectedResponse response;
  int err = soap_.__ns1__isConnected(&request, &response);
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

bool
ClientService::getAuthorized()
{
  DOUT("getAuthorized:enter");
  ns1__isAuthorized request;
  ns1__isAuthorizedResponse response;
  int err = soap_.__ns1__isAuthorized(&request, &response);
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

QString
ClientService::chat(const QString &message)
{
  DOUT("chat:enter");

  std::string arg0 = message.toStdString();

  ns1__chat request;
  request.arg0 = &arg0;

  ns1__chatResponse response;
  int err = soap_.__ns1__chat(&request, &response);
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
ClientService::getUser()
{
  DOUT("getUser:enter");
  User ret;

  ns1__getUser request;
  ns1__getUserResponse response;
  int err = soap_.__ns1__getUser(&request, &response);
  if (err) {
    DOUT("getUser: soap error, err =" << err);
    emit soapError(err);
    DOUT("getUser:exit");
    return ret;
  }

  ns1__user *p = response.return_;
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
    ret.setBannedCount(p->bannedCount);

    if (p->name)
      ret.setName(QString::fromStdString(*p->name));
    if (p->nickname)
      ret.setNickname(QString::fromStdString(*p->nickname));
    if (p->email)
      ret.setEmail(QString::fromStdString(*p->email));
    if (p->password)
      ret.setPassword(QString::fromStdString(*p->password));
  }

  DOUT("getUser:exit: username =" << ret.name() << ", id =" << ret.id());
  return ret;
}

qint64
ClientService::submitToken(const Token &token, const QString &alias)
{
  DOUT("submitToken:enter");

  if (!token.hasDigest() || token.digest().length() != Traits::MEDIA_TOKEN_DIGEST_LENGTH) {
    DOUT("submitToken:exit: error: invalid digest =" << token.digest());
    return 0;
  }
  if (alias.size() > Traits::MAX_MEDIA_TOKEN_ALIAS_LENGTH) {
    DOUT("submitToken:exit: error: alias too long, size =" << alias.size());
    return 0;
  }

  ns1__mediaToken arg0;
  arg0.status = token.status();
  arg0.flags = token.flags();
  arg0.id = token.id();
  arg0.userId = token.userId();
  std::string digest = token.digest().toStdString();
  arg0.digest = &digest;
  arg0.createTime = token.createTime();
  arg0.blessedCount = token.blessedCount();
  arg0.cursedCount = token.cursedCount();
  arg0.bannedCount = token.bannedCount();
  arg0.visitedCount = token.visitedCount();

  ns1__submitMediaToken request;
  request.arg0 = &arg0;

  std::string arg1;
  if (!alias.isEmpty()) {
    arg1 = alias.toStdString();
    request.arg1 = &arg1;
  }

  ns1__submitMediaTokenResponse response;
  int err = soap_.__ns1__submitMediaToken(&request, &response);
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
ClientService::submitAnnotation(const Annotation &annot)
{
  DOUT("submitAnnotation:enter");

  if (!annot.hasText()) {
    DOUT("submitAnnotation:exit: error: missing text");
    return 0;
  }
  if (annot.text().size() > Traits::MAX_MEDIA_ANNOT_TEXT_LENGTH) {
    DOUT("submitAnnotation:exit: error: text too long, size =" << annot.text().size());
    return 0;
  }

  ns1__mediaAnnotation arg0;
  arg0.status = annot.status();
  arg0.flags = annot.flags();
  arg0.id = annot.id();
  arg0.groupId = annot.groupId();
  arg0.tokenId = annot.tokenId();
  arg0.userId = annot.userId();
  std::string userAlias = annot.userAlias().toStdString();
  arg0.userAlias = &userAlias;
  arg0.pos = annot.pos();
  std::string text = annot.text().toStdString();
  arg0.text = &text;
  arg0.language = annot.language();
  arg0.createTime = annot.createTime();
  arg0.updateTime = annot.updateTime();
  arg0.blessedCount = annot.blessedCount();
  arg0.cursedCount = annot.cursedCount();
  arg0.bannedCount = annot.bannedCount();

  ns1__submitMediaAnnotation request;
  request.arg0 = &arg0;

  ns1__submitMediaAnnotationResponse response;
  int err = soap_.__ns1__submitMediaAnnotation(&request, &response);
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

Token
ClientService::selectTokenWithId(qint64 id)
{
  DOUT("selectTokenWithId:enter");
  Token ret;

  ns1__selectMediaTokenWithId request;
  request.arg0 = id;

  ns1__selectMediaTokenWithIdResponse response;
  int err = soap_.__ns1__selectMediaTokenWithId(&request, &response);
  if (err) {
    DOUT("selectTokenWithId: soap error, err =" << err);
    emit soapError(err);
    DOUT("selectTokenWithId:exit");
    return ret;
  }

  ns1__mediaToken *p = response.return_;
  if (p) {
    ret.setStatus(p->status);
    ret.setFlags(p->flags);
    ret.setId(p->id);
    ret.setUserId(p->userId);
    if (p->digest)
      ret.setDigest(QString::fromStdString(*p->digest));
    ret.setCreateTime(p->createTime);
    ret.setBlessedCount(p->blessedCount);
    ret.setCursedCount(p->cursedCount);
    ret.setBannedCount(p->bannedCount);
    ret.setVisitedCount(p->visitedCount);
  }

  DOUT("selectTokenWithId:exit: tid =" << ret.id());
  return ret;
}

Token
ClientService::selectTokenWithDigest(const QString &digest)
{
  DOUT("selectTokenWithId:enter");
  Token ret;

  ns1__selectMediaTokenWithDigest request;
  std::string arg0 = digest.toStdString();
  request.arg0 = &arg0;

  ns1__selectMediaTokenWithDigestResponse response;
  int err = soap_.__ns1__selectMediaTokenWithDigest(&request, &response);
  if (err) {
    DOUT("selectTokenWithId: soap error, err =" << err);
    emit soapError(err);
    DOUT("selectTokenWithId:exit");
    return ret;
  }

  ns1__mediaToken *p = response.return_;
  if (p) {
    ret.setStatus(p->status);
    ret.setFlags(p->flags);
    ret.setId(p->id);
    ret.setUserId(p->userId);
    ret.setDigest(digest);
    ret.setCreateTime(p->createTime);
    ret.setBlessedCount(p->blessedCount);
    ret.setCursedCount(p->cursedCount);
    ret.setBannedCount(p->bannedCount);
    ret.setVisitedCount(p->visitedCount);
  }

  DOUT("selectTokenWithId:exit: tid =" << ret.id());
  return ret;
}

AnnotationList
ClientService::selectAnnotationsWithTokenId(qint64 tid)
{
  DOUT("selectAnnotationsWithTokenId:enter: tid =" << tid);
  AnnotationList ret;

  ns1__selectMediaAnnotationsWithTokenId request;
  request.arg0 = tid;

  ns1__selectMediaAnnotationsWithTokenIdResponse response;
  int err = soap_.__ns1__selectMediaAnnotationsWithTokenId(&request, &response);
  if (err) {
    DOUT("selectAnnotationsWithTokenId: soap error, err =" << err);
    emit soapError(err);
    DOUT("selectAnnotationsWithTokenId:exit");
    return ret;
  }

  if (!response.return_.empty())
    foreach (ns1__mediaAnnotation *p, response.return_)
      if (p) {
        Annotation a;
        a.setStatus(p->status);
        a.setFlags(p->flags);
        a.setId(p->id);
        a.setGroupId(p->groupId);
        a.setTokenId(p->tokenId);
        a.setUserId(p->userId);
        if (p->userAlias)
          a.setUserAlias(QString::fromStdString(*p->userAlias));
        a.setPos(p->pos);
        if (p->text)
          a.setText(QString::fromStdString(*p->text));
        a.setCreateTime(p->createTime);
        a.setUpdateTime(p->updateTime);
        a.setBlessedCount(p->blessedCount);
        a.setCursedCount(p->cursedCount);
        a.setBannedCount(p->bannedCount);

        ret.append(a);
      }

  DOUT("selectAnnotationsWithTokenId:exit: count =" << ret.size());
  return ret;
}

// - Updates -

bool
ClientService::blessAnnotationWithId(qint64 id)
{
  DOUT("blessAnnotationWithId:enter: id =" << id);

  ns1__blessMediaAnnotationWithId request;
  request.arg0 = id;

  ns1__blessMediaAnnotationWithIdResponse response;
  int err = soap_.__ns1__blessMediaAnnotationWithId(&request, &response);
  if (err) {
    DOUT("blessAnnotationWithId: soap error, err =" << err);
    emit soapError(err);
    DOUT("blessAnnotationWithId:exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("blessAnnotationWithId:exit: ret =" << ret);
  return ret;
}
*/

// EOF
