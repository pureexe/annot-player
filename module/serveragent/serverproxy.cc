// module/serveragent/serverproxy.cc
// 9/10/2011
// See: http://blog.csdn.net/tingsking18/article/details/5456831

#include "serverproxy.h"
#include "serverproxy_config.h"
#include "core/data/traits.h"
#include <QtCore>

//#define DEBUG "ServerProxy"
#include "module/debug/debug.h"

using namespace Core::Data;
using namespace ServerSoap;

// - Construction -

ServerProxy::ServerProxy(QObject *parent)
  : Base(parent)
{ reset(); }

void
ServerProxy::reset()
{
  proxy_.reset(
    new ServerSoapProxy(SERVER_SOAP_MODE)
  );
#ifdef WITH_GZIP
  proxy_->z_level = 9; // compression level, default is 1 (fastest), max is 9
#endif // WIZH_GZIP
}

// - User -

bool
ServerProxy::setUserAnonymous(bool t)
{
  DOUT("setUserAnonymous:enter: anonymous =" << t);

  tns__setUserAnonymous request;
  request.arg0 = t;

  tns__setUserAnonymousResponse response;
  int err = proxy_->setUserAnonymous(&request, &response);
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
ServerProxy::setUserLanguage(qint32 language)
{
  DOUT("setUserLanguage:enter: language =" << language);

  tns__setUserLanguage request;
  request.arg0 = language;

  tns__setUserLanguageResponse response;
  int err = proxy_->setUserLanguage(&request, &response);
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
ServerProxy::setClientType(const QString &version)
{
  DOUT("setClientType:enter: version =" << version);

  tns__setClientType request;
  std::string arg0 = version.toStdString();
  request.arg0 = &arg0;

  tns__setClientTypeResponse response;
  int err = proxy_->setClientType(&request, &response);
  if (err) {
    DOUT("setClientType: soap error, err =" << err);
    emit soapError(err);
    DOUT("setClientType:exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("setClientType:exit: ret =" << ret);
  return ret;
}

// - Callback -

bool
ServerProxy::setCallback(int port, long key)
{
  DOUT("setCallback:enter: port =" << port << ", key =" << key);

  tns__setCallback request;
  request.arg0 = port;
  request.arg1 = key;

  tns__setCallbackResponse response;
  int err = proxy_->setCallback(&request, &response);
  if (err) {
    DOUT("setCallback: soap error, err =" << err);
    emit soapError(err);
    DOUT("setCallback:exit");
    return false;
  }

  bool ret = response.return_;
  DOUT("setCallback:exit: ret =" << ret);
  return ret;
}

// - Actions -

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
  int err = proxy_->login(&request, &response);
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
  int err = proxy_->logout(&request, &response);
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
ServerProxy::getConnected()
{
  DOUT("getConnected:enter");
  tns__isConnected request;
  tns__isConnectedResponse response;
  int err = proxy_->isConnected(&request, &response);
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
ServerProxy::getAuthorized()
{
  DOUT("getAuthorized:enter");
  tns__isAuthorized request;
  tns__isAuthorizedResponse response;
  int err = proxy_->isAuthorized(&request, &response);
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
ServerProxy::chat(const QString &message)
{
  DOUT("chat:enter");

  std::string arg0 = message.toStdString();

  tns__chat request;
  request.arg0 = &arg0;

  tns__chatResponse response;
  int err = proxy_->chat(&request, &response);
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
ServerProxy::getUser()
{
  DOUT("getUser:enter");
  User ret;

  tns__getUser request;
  tns__getUserResponse response;
  int err = proxy_->getUser(&request, &response);
  if (err) {
    DOUT("getUser: soap error, err =" << err);
    emit soapError(err);
    DOUT("getUser:exit");
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

// - Submissions -

#define MAKE_submitToken(_type, _Type) \
  qint64 \
  ServerProxy::submit##_Type##Token(const Token &token) \
  { \
    DOUT("submit" #_Type "Token:enter"); \
\
    if (!token.hasDigest() || token.digest().length() != Traits::TOKEN_DIGEST_LENGTH) { \
      DOUT("submit" #_Type "Token:exit: error: invalid digest =" << token.digest()); \
      return 0; \
    } \
\
    tns__##_type##Token arg0; \
    arg0.status = token.status(); \
    arg0.flags = token.flags(); \
    arg0.id = token.id(); \
    arg0.userId = token.userId(); \
    std::string digest = token.digest().toStdString(); \
    arg0.digest = &digest; \
    arg0.createTime = token.createTime(); \
    arg0.blessedCount = token.blessedCount(); \
    arg0.cursedCount = token.cursedCount(); \
    arg0.blockedCount = token.blockedCount(); \
    arg0.visitedCount = token.visitedCount(); \
\
    tns__submit##_Type##Token request; \
    request.arg0 = &arg0; \
\
    tns__submit##_Type##TokenResponse response; \
    int err = proxy_->submit##_Type##Token(&request, &response); \
    if (err) { \
      DOUT("submit" #_Type "Token: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("submit" #_Type "Token:exit"); \
      return 0; \
    } \
\
    qint64 ret = response.return_; \
    DOUT("submit" #_Type "Token:exit: tid =" << ret); \
    return ret; \
  }

  MAKE_submitToken(media, Media)
  MAKE_submitToken(game, Game)
#undef MAKE_submitToken

#define MAKE_submitTokenDigest(_type, _Type) \
  qint64 \
  ServerProxy::submit##_Type##TokenDigest(const QString &digest) \
  { \
    DOUT("submit" #_Type "TokenDigest:enter"); \
\
    if (digest.length() != Traits::TOKEN_DIGEST_LENGTH) { \
      DOUT("submit" #_Type "TokenDigest:exit: error: invalid digest =" << digest); \
      return 0; \
    } \
\
    tns__submit##_Type##TokenDigest request; \
    std::string arg0 = digest.toStdString(); \
    request.arg0 = &arg0; \
\
    tns__submit##_Type##TokenDigestResponse response; \
    int err = proxy_->submit##_Type##TokenDigest(&request, &response); \
    if (err) { \
      DOUT("submit" #_Type "TokenDigest: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("submit" #_Type "TokenDigest:exit"); \
      return 0; \
    } \
\
    qint64 ret = response.return_; \
    DOUT("submit" #_Type "Token:exit: tid =" << ret); \
    return ret; \
  }

  MAKE_submitTokenDigest(media, Media)
  MAKE_submitTokenDigest(game, Game)
#undef MAKE_submitTokenDigest

#define MAKE_submitAlias(_type, _Type) \
  qint64 \
  ServerProxy::submit##_Type##Alias(const Alias &alias) \
  { \
    DOUT("submit" #_Type "Alias:enter"); \
\
    if (!alias.hasText()) { \
      DOUT("submit" #_Type "Alias:exit: error: missing text"); \
      return 0; \
    } \
    if (alias.text().size() > Traits::MAX_ANNOT_LENGTH) { \
      DOUT("submit" #_Type "Alias:exit: error: text too long, size =" << alias.text().size()); \
      return 0; \
    } \
\
    tns__##_type##Alias arg0; \
    arg0.status = alias.status(); \
    arg0.flags = alias.flags(); \
    arg0.id = alias.id(); \
    arg0.tokenId = alias.tokenId(); \
    arg0.userId = alias.userId(); \
    arg0.type = alias.aliasType(); \
    std::string text = alias.text().toStdString(); \
    arg0.text = &text; \
    arg0.language = alias.language(); \
    arg0.updateTime = alias.updateTime(); \
    arg0.blessedCount = alias.blessedCount(); \
    arg0.cursedCount = alias.cursedCount(); \
    arg0.blockedCount = alias.blockedCount(); \
\
    tns__submit##_Type##Alias request; \
    request.arg0 = &arg0; \
\
    tns__submit##_Type##AliasResponse response; \
    int err = proxy_->submit##_Type##Alias(&request, &response); \
    if (err) { \
      DOUT("submit" #_Type "Alias: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("submitAlias:exit"); \
      return 0; \
    } \
\
    qint64 ret = response.return_; \
    DOUT("submit" #_Type "Alias:exit: aid =" << ret); \
    return ret; \
  }

  MAKE_submitAlias(media, Media)
  MAKE_submitAlias(game, Game)
#undef MAKE_submitAlias

#define MAKE_submitAliasTextWithTokenId(_type, _Type) \
  qint64 \
  ServerProxy::submit##_Type##AliasTextWithTokenId(const QString &text, qint32 aliasType, qint64 tokenId) \
  { \
    DOUT("submit" #_Type "AliasTextWithTokenId:enter"); \
\
    if (text.isEmpty()) { \
      DOUT("submit" #_Type "AliasTextWithTokenId:exit: error: missing text"); \
      return 0; \
    } \
    if (text.size() > Traits::MAX_ANNOT_LENGTH) { \
      DOUT("submit" #_Type "AliasTextWithTokenId:exit: error: text too long, size =" << text.size()); \
      return 0; \
    } \
\
    tns__submit##_Type##AliasTextWithTokenId request; \
    std::string arg0 = text.toStdString(); \
    request.arg0 = &arg0; \
    request.arg1 = aliasType; \
    request.arg2 = tokenId; \
\
    tns__submit##_Type##AliasTextWithTokenIdResponse response; \
    int err = proxy_->submit##_Type##AliasTextWithTokenId(&request, &response); \
    if (err) { \
      DOUT("submit" #_Type "AliasTextWithTokenId: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("submit" #_Type "AliasTextWithTokenId:exit"); \
      return 0; \
    } \
\
    qint64 ret = response.return_; \
    DOUT("submit" #_Type "AliasTextWithTokenId:exit: aid =" << ret); \
    return ret; \
  }

  MAKE_submitAliasTextWithTokenId(media, Media)
  MAKE_submitAliasTextWithTokenId(game, Game)
#undef MAKE_submitAliasTextWithTokenId

#define MAKE_submitAliasTextAndTokenDigest(_type, _Type) \
  qint64 \
  ServerProxy::submit##_Type##AliasTextAndTokenDigest(const QString &text, qint32 aliasType, const QString &digest) \
  { \
    DOUT("submit" #_Type "AliasTextAndTokenDigest:enter"); \
\
    if (text.isEmpty()) { \
      DOUT("submit" #_Type "AliasTextAndTokenDigest:exit: error: missing text"); \
      return 0; \
    } \
    if (text.size() > Traits::MAX_ANNOT_LENGTH) { \
      DOUT("submit" #_Type "AliasTextAndTokenDigest:exit: error: text too long, size =" << text.size()); \
      return 0; \
    } \
    if (digest.size() != Traits::TOKEN_DIGEST_LENGTH) { \
      DOUT("submit" #_Type "AliasTextAndTokenDigest:exit: invalid digest size =" << digest.size()); \
      return 0; \
    } \
\
    tns__submit##_Type##AliasTextAndTokenDigest request; \
    std::string arg0 = text.toStdString(); \
    std::string arg2 = digest.toStdString(); \
    request.arg0 = &arg0; \
    request.arg1 = aliasType; \
    request.arg2 = &arg2; \
\
    tns__submit##_Type##AliasTextAndTokenDigestResponse response; \
    int err = proxy_->submit##_Type##AliasTextAndTokenDigest(&request, &response); \
    if (err) { \
      DOUT("submit" #_Type "AliasTextAndTokenDigest: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("submit" #_Type "AliasTextAndTokenDigest:exit"); \
      return 0; \
    } \
\
    qint64 ret = response.return_; \
    DOUT("submit" #_Type "AliasTextAndTokenDigest:exit: aid =" << ret); \
    return ret; \
  }

  MAKE_submitAliasTextAndTokenDigest(media, Media)
  MAKE_submitAliasTextAndTokenDigest(game, Game)
#undef MAKE_submitAliasTextAndTokenDigest

#define MAKE_submitAnnotation(_type, _Type) \
  qint64 \
  ServerProxy::submit##_Type##Annotation(const Annotation &annot) \
  { \
    DOUT("submit" #_Type "Annotation:enter"); \
\
    if (!annot.hasText()) { \
      DOUT("submit" #_Type "Annotation:exit: error: missing text"); \
      return 0; \
    } \
    if (annot.text().size() > Traits::MAX_ANNOT_LENGTH) { \
      DOUT("submit" #_Type "Annotation:exit: error: text too long, size =" << annot.text().size()); \
      return 0; \
    } \
\
    tns__##_type##Annotation arg0; \
    arg0.status = annot.status(); \
    arg0.flags = annot.flags(); \
    arg0.id = annot.id(); \
    arg0.tokenId = annot.tokenId(); \
    arg0.userId = annot.userId(); \
    std::string userAlias = annot.userAlias().toStdString(); \
    arg0.userAlias = &userAlias; \
    arg0.pos = annot.pos(); \
    arg0.posType = annot.posType(); \
    std::string text = annot.text().toStdString(); \
    arg0.text = &text; \
    arg0.language = annot.language(); \
    arg0.createTime = annot.createTime(); \
    arg0.updateTime = annot.updateTime(); \
    arg0.blessedCount = annot.blessedCount(); \
    arg0.cursedCount = annot.cursedCount(); \
    arg0.blockedCount = annot.blockedCount(); \
\
    tns__submit##_Type##Annotation request; \
    request.arg0 = &arg0; \
\
    tns__submit##_Type##AnnotationResponse response; \
    int err = proxy_->submit##_Type##Annotation(&request, &response); \
    if (err) { \
      DOUT("submit" #_Type "Annotation: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("submit" #_Type "Annotation:exit"); \
      return 0; \
    } \
\
    qint64 ret = response.return_; \
    DOUT("submit" #_Type "Annotation:exit: aid =" << ret); \
    return ret; \
  }

  MAKE_submitAnnotation(media, Media)
  MAKE_submitAnnotation(game, Game)
#undef MAKE_submitAnnotation

#define MAKE_submitAnnotationTextWithTokenId(_type, _Type) \
  qint64 \
  ServerProxy::submit##_Type##AnnotationTextWithTokenId(const QString &text, qint64 pos, qint32 posType, qint64 tokenId) \
  { \
    DOUT("submit" #_Type "AnnotationTextWithTokenId:enter"); \
\
    if (text.isEmpty()) { \
      DOUT("submit" #_Type "AnnotationTextWithTokenId:exit: error: missing text"); \
      return 0; \
    } \
    if (text.size() > Traits::MAX_ANNOT_LENGTH) { \
      DOUT("submit" #_Type "AnnotationTextWithTokenId:exit: error: text too long, size =" << text.size()); \
      return 0; \
    } \
\
    tns__submit##_Type##AnnotationTextWithTokenId request; \
    std::string arg0 = text.toStdString(); \
    request.arg0 = &arg0; \
    request.arg1 = pos; \
    request.arg2 = posType; \
    request.arg3 = tokenId; \
\
    tns__submit##_Type##AnnotationTextWithTokenIdResponse response; \
    int err = proxy_->submit##_Type##AnnotationTextWithTokenId(&request, &response); \
    if (err) { \
      DOUT("submit" #_Type "AnnotationTextWithTokenId: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("submit" #_Type "AnnotationTextWithTokenId:exit"); \
      return 0; \
    } \
\
    qint64 ret = response.return_; \
    DOUT("submit" #_Type "AnnotationTextWithTokenId:exit: aid =" << ret); \
    return ret; \
  }

  MAKE_submitAnnotationTextWithTokenId(media, Media)
  MAKE_submitAnnotationTextWithTokenId(game, Game)
#undef MAKE_submitAnnotationText

#define MAKE_submitAnnotationTextAndTokenDigest(_type, _Type) \
  qint64 \
  ServerProxy::submit##_Type##AnnotationTextAndTokenDigest(const QString &text, qint64 pos, qint32 posType, const QString &digest) \
  { \
    DOUT("submit" #_Type "AnnotationTextAndTokenDigest:enter"); \
\
    if (text.isEmpty()) { \
      DOUT("submit" #_Type "AnnotationTextAndTokenDigest:exit: error: missing text"); \
      return 0; \
    } \
    if (text.size() > Traits::MAX_ANNOT_LENGTH) { \
      DOUT("submit" #_Type "AnnotationTextAndTokenDigest:exit: error: text too long, size =" << text.size()); \
      return 0; \
    } \
    if (digest.size() != Traits::TOKEN_DIGEST_LENGTH) { \
      DOUT("submit" #_Type "AnnotationTextAndTokenDigest:exit: error: text too long, size =" << text.size()); \
      return 0; \
    } \
\
    tns__submit##_Type##AnnotationTextAndTokenDigest request; \
    std::string arg0 = text.toStdString(); \
    std::string arg3 = digest.toStdString(); \
    request.arg0 = &arg0; \
    request.arg1 = pos; \
    request.arg2 = posType; \
    request.arg3 = &arg3; \
\
    tns__submit##_Type##AnnotationTextAndTokenDigestResponse response; \
    int err = proxy_->submit##_Type##AnnotationTextAndTokenDigest(&request, &response); \
    if (err) { \
      DOUT("submit" #_Type "AnnotationTextAndTokenDigest: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("submit" #_Type "AnnotationTextAndTokenDigest:exit"); \
      return 0; \
    } \
\
    qint64 ret = response.return_; \
    DOUT("submit" #_Type "AnnotationTextAndTokenDigest:exit: aid =" << ret); \
    return ret; \
  }

  MAKE_submitAnnotationTextAndTokenDigest(media, Media)
  MAKE_submitAnnotationTextAndTokenDigest(game, Game)
#undef MAKE_submitAnnotationText

// - Queries -

#define MAKE_selectTokenWithId(_type, _Type) \
  Token \
  ServerProxy::select##_Type##TokenWithId(qint64 id) \
  { \
    DOUT("select" #_Type "TokenWithId:enter"); \
    Token ret; \
\
    tns__select##_Type##TokenWithId request; \
    request.arg0 = id; \
\
    tns__select##_Type##TokenWithIdResponse response; \
    int err = proxy_->select##_Type##TokenWithId(&request, &response); \
    if (err) { \
      DOUT("select" #_Type "TokenWithId: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("select" #_Type "TokenWithId:exit"); \
      return ret; \
    } \
\
    tns__##_type##Token *p = response.return_; \
    if (p) { \
      ret.setType(Traits::_Type##Type); \
      ret.setStatus(p->status); \
      ret.setFlags(p->flags); \
      ret.setId(p->id); \
      ret.setUserId(p->userId); \
      if (p->digest) \
        ret.setDigest(QString::fromStdString(*p->digest)); \
      ret.setCreateTime(p->createTime); \
      ret.setBlessedCount(p->blessedCount); \
      ret.setCursedCount(p->cursedCount); \
      ret.setBlockedCount(p->blockedCount); \
      ret.setVisitedCount(p->visitedCount); \
    } \
\
    DOUT("select" #_Type "TokenWithId:exit: tid =" << ret.id()); \
    return ret; \
  }

  MAKE_selectTokenWithId(media, Media)
  MAKE_selectTokenWithId(game, Game)
#undef MAKE_selectTokenWithId

#define MAKE_selectTokenWithDigest(_type, _Type) \
  Token \
  ServerProxy::select##_Type##TokenWithDigest(const QString &digest) \
  { \
    DOUT("select" #_Type "TokenWithDigest:enter"); \
    Token ret; \
\
    tns__select##_Type##TokenWithDigest request; \
    std::string arg0 = digest.toStdString(); \
    request.arg0 = &arg0; \
\
    tns__select##_Type##TokenWithDigestResponse response; \
    int err = proxy_->select##_Type##TokenWithDigest(&request, &response); \
    if (err) { \
      DOUT("select" #_Type "TokenWithDigest: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("select" #_Type "TokenWithDigest:exit"); \
      return ret; \
    } \
\
    tns__##_type##Token *p = response.return_; \
    if (p) { \
      ret.setType(Traits::_Type##Type); \
      ret.setStatus(p->status); \
      ret.setFlags(p->flags); \
      ret.setId(p->id); \
      ret.setUserId(p->userId); \
      ret.setDigest(digest); \
      ret.setCreateTime(p->createTime); \
      ret.setBlessedCount(p->blessedCount); \
      ret.setCursedCount(p->cursedCount); \
      ret.setBlockedCount(p->blockedCount); \
      ret.setVisitedCount(p->visitedCount); \
    } \
\
    DOUT("select" #_Type "TokenWithId:exit: tid =" << ret.id()); \
    return ret; \
  }

  MAKE_selectTokenWithDigest(media, Media)
  MAKE_selectTokenWithDigest(game, Game)
#undef MAKE_selectTokenWithDigest

#define MAKE_selectAliasesWithTokenId(_type, _Type) \
  AliasList \
  ServerProxy::select##_Type##AliasesWithTokenId(qint64 tid) \
  { \
    DOUT("select" #_Type "AliasesWithTokenId:enter: tid =" << tid); \
    AliasList ret; \
\
    tns__select##_Type##AliasesWithTokenId request; \
    request.arg0 = tid; \
\
    tns__select##_Type##AliasesWithTokenIdResponse response; \
    int err = proxy_->select##_Type##AliasesWithTokenId(&request, &response); \
    if (err) { \
      DOUT("select" #_Type "AliasesWithTokenId: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("select" #_Type "AliasesWithTokenId:exit"); \
      return ret; \
    } \
\
    if (!response.return_.empty()) \
      foreach (tns__##_type##Alias *p, response.return_) \
        if (p) { \
          Alias a; \
          a.setType(Traits::_Type##Type); \
          a.setStatus(p->status); \
          a.setFlags(p->flags); \
          a.setId(p->id); \
          a.setTokenId(p->tokenId); \
          a.setUserId(p->userId); \
          a.setAliasType(p->type); \
          a.setLanguage(p->language); \
          if (p->text) \
            a.setText(QString::fromStdString(*p->text)); \
          a.setUpdateTime(p->updateTime); \
          a.setBlessedCount(p->blessedCount); \
          a.setCursedCount(p->cursedCount); \
          a.setBlockedCount(p->blockedCount); \
\
          ret.append(a); \
        } \
\
    DOUT("select" #_Type "AliasesWithTokenId:exit: count =" << ret.size()); \
    return ret; \
  }

  MAKE_selectAliasesWithTokenId(media, Media)
  MAKE_selectAliasesWithTokenId(game, Game)
#undef MAKE_selectAliasesWithTokenId

#define MAKE_selectAnnotationsWithTokenId(_type, _Type) \
  AnnotationList \
  ServerProxy::select##_Type##AnnotationsWithTokenId(qint64 tid) \
  { \
    DOUT("select" #_Type "AnnotationsWithTokenId:enter: tid =" << tid); \
    AnnotationList ret; \
\
    tns__select##_Type##AnnotationsWithTokenId request; \
    request.arg0 = tid; \
\
    tns__select##_Type##AnnotationsWithTokenIdResponse response; \
    int err = proxy_->select##_Type##AnnotationsWithTokenId(&request, &response); \
    if (err) { \
      DOUT("select" #_Type "AnnotationsWithTokenId: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("select" #_Type "AnnotationsWithTokenId:exit"); \
      return ret; \
    } \
\
    if (!response.return_.empty()) \
      foreach (tns__##_type##Annotation *p, response.return_) \
        if (p) { \
          Annotation a; \
          a.setType(Traits::_Type##Type); \
          a.setStatus(p->status); \
          a.setFlags(p->flags); \
          a.setId(p->id); \
          a.setTokenId(p->tokenId); \
          a.setUserId(p->userId); \
          if (p->userAlias) \
            a.setUserAlias(QString::fromStdString(*p->userAlias)); \
          a.setPos(p->pos); \
          a.setPosType(p->posType); \
          a.setLanguage(p->language); \
          if (p->text) \
            a.setText(QString::fromStdString(*p->text)); \
          a.setCreateTime(p->createTime); \
          a.setUpdateTime(p->updateTime); \
          a.setBlessedCount(p->blessedCount); \
          a.setCursedCount(p->cursedCount); \
          a.setBlockedCount(p->blockedCount); \
\
          ret.append(a); \
        } \
\
    DOUT("select" #_Type "AnnotationsWithTokenId:exit: count =" << ret.size()); \
    return ret; \
  }

  MAKE_selectAnnotationsWithTokenId(media, Media)
  MAKE_selectAnnotationsWithTokenId(game, Game)
#undef MAKE_selectAnnotationsWithTokenId

#define MAKE_selectRelatedAnnotationsWithTokenId(_type, _Type) \
  AnnotationList \
  ServerProxy::selectRelated##_Type##AnnotationsWithTokenId(qint64 tid) \
  { \
    DOUT("selectRelated" #_Type "AnnotationsWithTokenId:enter: tid =" << tid); \
    AnnotationList ret; \
\
    tns__selectRelated##_Type##AnnotationsWithTokenId request; \
    request.arg0 = tid; \
\
    tns__selectRelated##_Type##AnnotationsWithTokenIdResponse response; \
    int err = proxy_->selectRelated##_Type##AnnotationsWithTokenId(&request, &response); \
    if (err) { \
      DOUT("selectRelated" #_Type "AnnotationsWithTokenId: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("selectRelated" #_Type "AnnotationsWithTokenId:exit"); \
      return ret; \
    } \
\
    if (!response.return_.empty()) \
      foreach (tns__##_type##Annotation *p, response.return_) \
        if (p) { \
          Annotation a; \
          a.setType(Traits::_Type##Type); \
          a.setStatus(p->status); \
          a.setFlags(p->flags); \
          a.setId(p->id); \
          a.setTokenId(p->tokenId); \
          a.setUserId(p->userId); \
          if (p->userAlias) \
            a.setUserAlias(QString::fromStdString(*p->userAlias)); \
          a.setPos(p->pos); \
          a.setPosType(p->posType); \
          a.setLanguage(p->language); \
          if (p->text) \
            a.setText(QString::fromStdString(*p->text)); \
          a.setCreateTime(p->createTime); \
          a.setUpdateTime(p->updateTime); \
          a.setBlessedCount(p->blessedCount); \
          a.setCursedCount(p->cursedCount); \
          a.setBlockedCount(p->blockedCount); \
\
          ret.append(a); \
        } \
\
    DOUT("selectRelated" #_Type "AnnotationsWithTokenId:exit: count =" << ret.size()); \
    return ret; \
  }

  MAKE_selectRelatedAnnotationsWithTokenId(media, Media)
  MAKE_selectRelatedAnnotationsWithTokenId(game, Game)
#undef MAKE_selectRelatedAnnotationsWithTokenId

// - Cast -

#define MAKE_blessAnnotationWithId(_type, _Type) \
  bool \
  ServerProxy::bless##_Type##AnnotationWithId(qint64 id) \
  { \
    DOUT("bless" #_Type "AnnotationWithId:enter: id =" << id); \
\
    tns__bless##_Type##AnnotationWithId request; \
    request.arg0 = id; \
\
    tns__bless##_Type##AnnotationWithIdResponse response; \
    int err = proxy_->bless##_Type##AnnotationWithId(&request, &response); \
    if (err) { \
      DOUT("bless" #_Type "AnnotationWithId: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("bless" #_Type "AnnotationWithId:exit"); \
      return false; \
    } \
\
    bool ret = response.return_; \
    DOUT("bless" #_Type "AnnotationWithId:exit: ret =" << ret); \
    return ret; \
  }

  MAKE_blessAnnotationWithId(media, Media)
  MAKE_blessAnnotationWithId(game, Game)
#undef MAKE_blessAnnotationWithId

// - Update -

#define MAKE_updateMediaAnnotationTextWithId(_type, _Type) \
  bool \
  ServerProxy::update##_Type##AnnotationTextWithId(const QString &text, qint64 id) \
  { \
    DOUT("update" #_Type "AnnotationTextWithId:enter: id =" << id << ", text =" << text); \
\
    tns__update##_Type##AnnotationTextWithId request; \
    std::string arg0 = text.toStdString(); \
    request.arg0 = &arg0; \
    request.arg1 = id; \
\
    tns__update##_Type##AnnotationTextWithIdResponse response; \
    int err = proxy_->update##_Type##AnnotationTextWithId(&request, &response); \
    if (err) { \
      DOUT("update" #_Type "AnnotationTextWithId: soap error, err =" << err); \
      emit soapError(err); \
      DOUT("update" #_Type "AnnotationTextWithId:exit"); \
      return false; \
    } \
\
    bool ret = response.return_; \
    DOUT("update" #_Type "AnnotationTextWithId:exit: ret =" << ret); \
    return ret; \
  } \

  MAKE_updateMediaAnnotationTextWithId(media, Media)
  MAKE_updateMediaAnnotationTextWithId(game, Game)
#undef MAKE_updateMediaAnnotationTextWithId

// EOF
