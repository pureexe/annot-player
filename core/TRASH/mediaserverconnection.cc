// core/mediaserverconnection.cc
// 8/4/2011

#include "core/mediaserverconnection.h"
#include "core/mediaserver.h"
#include <QtCore>
#include <QtNetwork>
#include <QtXml>

#define TR_ERROR_SUCCEED        tr("succeed")   // TO BE MOVED TO OTHER PLACE WITH A BETTER NAME
#define TR_ERROR_INVALID_USERNAME_OR_PASSWORD  tr("MediaServerConnection: Invalid user name or password")

//#define DEBUG

#ifdef DEBUG
  #define DOUT(_msg)    qDebug() << "MediaServerConnection:" << _msg
#else
  #define DOUT(_dummy)

  #ifdef _MSC_VER
    #pragma warning (disable: 4390)     // C4390: empty controlled statement found: is this the intent?
  #endif // _MSC_VER

  #ifdef __GNUC__
    #pragma GCC diagnostic ignored "-Wempty-body" // empty body in an if or else statement
  #endif // __GNUC__
#endif // DEBUG

/*
namespace { // anonymous, xml helper - TO BE MOVED TO ANOTHER CORE FILE!

  QList<QDomNode>
  elementsByTagName(const QDomNode &node, const QString &tagName)
  {
    QList<QDomNode> ret;
    QDomNodeList childNodes = node.toElement().elementsByTagName(tagName);
    for (int i = 0; i < childNodes.count(); i++) {
      QDomNode n = childNodes.item(i);
      if (n.parentNode() == node)
        ret.append(n);
    }
    return ret;
  }

} // anonymous namespace
*/

namespace Core {

  // - Constructions -

  MediaServerConnection::MediaServerConnection(QObject *parent)
    : Base(parent), login_(false)
  {
    qnam_ = new QNetworkAccessManager(this);
    connect(qnam_, SIGNAL(finished(QNetworkReply*)), SLOT(processServerReply(QNetworkReply*)));
  }

  MediaServerConnection::~MediaServerConnection() { }

  // - Login -

  const MediaServerConnection::User&
  MediaServerConnection::user() const
  { return user_; }

  void
  MediaServerConnection::setUser(const User &user)
  { user_ = user; }

  qint64
  MediaServerConnection::userId() const
  { return user_.id(); }

  const QString&
  MediaServerConnection::userName() const
  { return user_.name(); }

  const QString&
  MediaServerConnection::userPassword() const
  { return user_.password(); }

  bool
  MediaServerConnection::isLoggedIn() const
  { return login_; }

  // - Logon -
  void
  MediaServerConnection::login(const QString &userName, const QString &password)
  {
    //if (login_)
    //  logout();
    login_ = false;

    user_.setName(userName.toLatin1());
    if (User::isValidPassword(password))
      user_.setPassword(User::encryptPassword(password.toLatin1()));

    login();
  }

  void
  MediaServerConnection::login()
  {
    emit loginRequested();
    if (!user_.hasValidName() || !user_.hasPassword()) {
      qDebug() << TR_ERROR_INVALID_USERNAME_OR_PASSWORD;
      emit loginFailed();
      return;
    }

    // TODO:connect signal based on if login!
    QUrl query(CORE_MS_API_LOGIN);
    query.addEncodedQueryItem(CORE_MS_QUERY_USERNAME, QUrl::toPercentEncoding(user_.name()));
    query.addEncodedQueryItem(CORE_MS_QUERY_PASSWORD, QUrl::toPercentEncoding(user_.password()));

    DOUT(query.toEncoded());
    qnam_->get(QNetworkRequest(query));
  }

  void
  MediaServerConnection::logout()
  {
    if (!login_) {
      DOUT("Double-logout");
    }

    emit logoutRequested();

    QUrl query(CORE_MS_API_LOGOUT);

    DOUT(query.toEncoded());
    qnam_->get(QNetworkRequest(query));

    user_.clear();
    login_ = false;
    emit logoutFinished();
  }

  // - Queries -

  QString
  MediaServerConnection::errorToString(int error) const
  {
    if (error == 0)
      return TR_ERROR_SUCCEED;
    else
      return TR_ERROR_SUCCEED;   // TODO
  }

  void
  MediaServerConnection::submitToken(const MediaToken &token, const QString& source)
  {
    Q_ASSERT(qnam_);

    QUrl query(CORE_MS_API_TOKEN);
    query.addQueryItem(CORE_MS_QUERY_DOMAIN, CORE_MS_DOMAIN_MEDIA);
    query.addQueryItem(CORE_MS_QUERY_ACTION, CORE_MS_ACTION_SUBMIT);
    if (token.hasId())
      query.addQueryItem(CORE_MS_QUERY_TOKENID, QString::number(token.id()));
    if (token.hasDigest())
      query.addQueryItem(CORE_MS_QUERY_DIGEST, QString(token.digest()));
    if (token.hasCreateTime())
      query.addQueryItem(CORE_MS_QUERY_CREATETIME, QString::number(token.createTime()));
    if (!source.isEmpty())
      query.addEncodedQueryItem(CORE_MS_QUERY_SOURCE, QUrl::toPercentEncoding(source));

    DOUT(query.toEncoded());
    qnam_->get(QNetworkRequest(query));
  }

  void
  MediaServerConnection::submitAnnotation(const MediaAnnotation &annot)
  {
    Q_ASSERT(qnam_);

    QUrl query(CORE_MS_API_ANNOT);
    query.addQueryItem(CORE_MS_QUERY_DOMAIN, CORE_MS_DOMAIN_MEDIA);
    query.addQueryItem(CORE_MS_QUERY_ACTION, CORE_MS_ACTION_SUBMIT);
    if (annot.hasId())
      query.addQueryItem(CORE_MS_QUERY_ANNOTID, QString::number(annot.id()));
    if (annot.hasTokenId())
      query.addQueryItem(CORE_MS_QUERY_TOKENID, QString::number(annot.tokenId()));
    if (annot.hasUserId())
      query.addQueryItem(CORE_MS_QUERY_USERID, QString::number(annot.userId()));
    if (annot.hasCreateTime())
      query.addQueryItem(CORE_MS_QUERY_CREATETIME, QString::number(annot.createTime()));
    if (annot.hasUpdateTime())
      query.addQueryItem(CORE_MS_QUERY_UPDATETIME, QString::number(annot.updateTime()));
    if (annot.hasLanguages())
      query.addQueryItem(CORE_MS_QUERY_LANGUAGE, QString::number(annot.languages()));
    if (annot.hasPos())
      query.addQueryItem(CORE_MS_QUERY_POS, QString::number(annot.pos()));
    if (annot.hasText())
      query.addEncodedQueryItem(CORE_MS_QUERY_TEXT, QUrl::toPercentEncoding(annot.text()));

    DOUT(query.toEncoded());
    qnam_->get(QNetworkRequest(query)); // TODO: Connect after login!!
  }

  void
  MediaServerConnection::queryClusteredTokensByTokenId(qint64 tid)
  {
    Q_ASSERT(qnam_);

    QUrl query(CORE_MS_API_TOKEN);
    query.addQueryItem(CORE_MS_QUERY_DOMAIN, CORE_MS_DOMAIN_MEDIA);
    query.addQueryItem(CORE_MS_QUERY_ACTION, CORE_MS_ACTION_CLUSTER);
    query.addQueryItem(CORE_MS_QUERY_TOKENID, QString::number(tid));

    DOUT(query.toEncoded());

    qnam_->get(QNetworkRequest(query));
  }

  /*
  void
  MediaServerConnection::queryClusteredTokensByDigest(const QString &digest)
  {
    Q_ASSERT(qnam_);
    if (digest.isNull()) {
      DOUT("MediaServerConnection: Null digest");
      return;
    }

    QUrl query(CORE_MS_API_TOKEN);
    query.addQueryItem(CORE_MS_QUERY_DOMAIN, CORE_MS_DOMAIN_MEDIA);
    query.addQueryItem(CORE_MS_QUERY_ACTION, CORE_MS_ACTION_CLUSTER);
    query.addQueryItem(CORE_MS_QUERY_DIGEST, QString(digest));

    DOUT(query.toEncoded());

    qnam_->get(QNetworkRequest(query));
  }
  */

  void
  MediaServerConnection::queryAnnotationsByTokenId(qint64 tid)
  {
    Q_ASSERT(qnam_);

    QUrl query(CORE_MS_API_ANNOT);
    query.addQueryItem(CORE_MS_QUERY_DOMAIN, CORE_MS_DOMAIN_MEDIA);
    query.addQueryItem(CORE_MS_QUERY_ACTION, CORE_MS_ACTION_QUERY);
    query.addQueryItem(CORE_MS_QUERY_TOKENID, QString::number(tid));

    DOUT(query.toEncoded());

    qnam_->get(QNetworkRequest(query));
  }

  // - Reply parser -

  void
  MediaServerConnection::processServerReply(QNetworkReply *reply)
  {
    Q_ASSERT(reply);
    reply->deleteLater();

    if (!reply->isFinished()
        || reply->error() != QNetworkReply::NoError) {
      DOUT(reply->error());
      return;
    }

    QDomDocument doc;
    doc.setContent(reply->readAll());
    if (doc.isNull()) {
      DOUT("Invalid document root");
      return;
    }

    QString rootTag = doc.documentElement().tagName();
    if (rootTag == CORE_MS_TAG_UNIVERSE) {
      QString childTag = doc.documentElement().firstChildElement().tagName();
      if (childTag == CORE_MS_TAG_MEDIAANNOT) {
        MediaAnnotationList annots = parseAnnotations(doc);
        if (!annots.empty())
          emit annotationsReceived(annots);
      } else if (childTag == CORE_MS_TAG_MEDIATOKEN) {
        MediaTokenList tokens = parseTokens(doc);
        if (!tokens.empty())
          emit tokensReceived(tokens);
      } else if (childTag == CORE_MS_TAG_MEDIAUSER) {
        // FIXME: The logic here is messed up.
        UserList users = parseUsers(doc);
        if (users.size() == 1) {
          QString password = user_.password();
          user_ = users.front();
          user_.setPassword(password);
          if (user_.hasId()) {
            login_ = true;
            emit loginSucceeded();
          } else {
            DOUT("Failed to get user id from login page");
            login_ = false;
            emit loginFailed();
          }
        } else {
          DOUT("Multiple users received - not implemented");
          login_ = false;
          emit loginFailed();
        }
      } else {
        DOUT("Unknown universe document type: " + childTag);
        login_ = false;
        emit loginFailed();
      }

    } else if (rootTag == CORE_MS_TAG_ERROR) {
      int error = parseError(doc);
      if (error)
        emit errorReceived(error);

    } else
      DOUT("Unknown document root: " + rootTag);
  }


  int
  MediaServerConnection::parseError(const QDomDocument &doc)
  {
    enum { INVALID_DOC = -1 }; // TODO: Move to header!
    int ret = INVALID_DOC;
    bool ok;
    if (doc.isNull()) {
      DOUT("Invalid error document root");
      return ret;
    }

    QDomElement root = doc.documentElement();

    // <ErrorCode>
    {
      QDomElement element = doc.firstChildElement(CORE_MS_TAG_ERRORCODE);
      if (element.isNull())
        DOUT("Missing error code");
      else {
        int value = element.text().toInt(&ok);
        if (!ok)
          qDebug() << "MediaServerConnection: Invalid error code: " << element.text();
        else
          ret = value;
      }
    }
#ifndef DEBUG
    if (ret) { // Only output error
#endif // DEBUG

      #define PARSE_STRING(_tag) \
        { \
          QDomElement element = root.firstChildElement(_tag); \
          if (element.isNull()) \
            DOUT("Missing " #_tag); \
          else \
            qDebug() << "MediaServerConnection: " #_tag ": " << element.text(); \
        }

        PARSE_STRING(CORE_MS_TAG_ERRORREASON)
        PARSE_STRING(CORE_MS_TAG_ERRORDETAIL)
        PARSE_STRING(CORE_MS_TAG_ERRORHELP)
      #undef PARSE_STRING

#ifndef DEBUG
    }
#endif // DEBUG

    return ret;
  }

  MediaServerConnection::UserList
  MediaServerConnection::parseUsers(const QDomDocument &doc)
  {
    UserList users;
    if (doc.isNull()) {
      DOUT("Invalid annotation document root");
      return users;
    }

    QDomNodeList n_users = doc.elementsByTagName(CORE_MS_TAG_USER);
    if (n_users.size() <= 0) {
      DOUT("No users found");
      return users;
    }

    for (int j = 0; j < n_users.size(); j++) {
      QDomElement e_user = n_users.at(j).toElement();
      if (e_user.isNull()) {
        DOUT("Invalid user element");
        continue;
      }

      User user;

      // <id>
      {
        QString s_id = e_user.attribute(CORE_MS_ATTRIBUTE_ID);
        if (s_id.isNull()) {
          DOUT("Invalid userId");
          continue; // Continue for invalidId
        }
        bool ok;
        qint64 id = s_id.toLongLong(&ok);
        user.setId(id);
        if (!ok) {
           DOUT("Invalid user ID attribute:" << s_id);
           continue;
        }
        user.setId(id);
      }

      QDomNode node = e_user.firstChild();
      while (!node.isNull()) {
        QDomElement element = node.toElement();
        QString tagName = element.tagName();

        if (tagName.isEmpty())
          continue;

      #define ELIF_LONGLONG(_property, _tag) \
        else if (tagName == _tag) { \
          bool ok; \
          qint64 value = element.text().toLongLong(&ok); \
          if (!ok) \
            DOUT("Invalid " #_property); \
          else \
            user.set##_property(value); \
        }

        ELIF_LONGLONG(CreateTime,CORE_MS_TAG_CREATETIME)
        ELIF_LONGLONG(LoginTime, CORE_MS_TAG_LOGINTIME)
      #undef ELIF_LONGLONG

      #define ELIF_UINT32(_property, _tag) \
        else if (tagName == _tag) { \
          bool ok; \
          uint value = element.text().toUInt(&ok); \
          if (!ok) \
            DOUT("Invalid " #_property); \
          else \
            user.set##_property((quint32)value); \
        }

        ELIF_UINT32(Languages, CORE_MS_TAG_LANGUAGE)

        ELIF_UINT32(BlessedCount, CORE_MS_TAG_BLESSEDCOUNT)
        ELIF_UINT32(CursedCount,  CORE_MS_TAG_CURSEDCOUNT)
      #undef ELIF_UINT32

      #define ELIF_INT(_property, _tag) \
        else if (tagName == _tag) { \
          bool ok; \
          int value = element.text().toInt(&ok); \
          if (!ok) \
            DOUT("Invalid " #_property); \
          else \
            user.set##_property(value); \
        }

        ELIF_INT(Score, CORE_MS_TAG_SCORE)
        ELIF_INT(Level, CORE_MS_TAG_LEVEL)
      #undef ELIF_INT

      #define ELIF_STRING(_property, _tag) \
        else if (tagName == _tag) { \
          user.set##_property(element.text()); \
        }

        ELIF_STRING(Name,  CORE_MS_TAG_NAME)
        ELIF_STRING(Email, CORE_MS_TAG_EMAIL)
        ELIF_STRING(Nickname, CORE_MS_TAG_NICKNAME)
      #undef ELIF_STRING

        else
          DOUT("parseUsers: Unprocessed tagName:" << tagName);

        node = node.nextSibling();
      }

      /*
      #define PARSE_LONGLONG(_property, _tag) \
        { \
          QDomElement elem = e_user.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else { \
            qint64 value = elem.text().toLongLong(&ok); \
            if (!ok) \
              DOUT("Invalid " #_property); \
            else \
              user.set##_property(value); \
          } \
        }

        PARSE_LONGLONG(CreateTime,CORE_MS_TAG_CREATETIME)
        PARSE_LONGLONG(LoginTime, CORE_MS_TAG_LOGINTIME)
      #undef PARSE_LONGLONG

      #define PARSE_UINT32(_property, _tag) \
        { \
          QDomElement elem = e_user.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else { \
            uint value = elem.text().toUInt(&ok); \
            if (!ok) \
              DOUT("Invalid " #_property); \
            else \
              user.set##_property((quint32)value); \
          } \
        }

        PARSE_UINT32(Languages, CORE_MS_TAG_LANGUAGE)

        PARSE_UINT32(BlessedCount, CORE_MS_TAG_BLESSEDCOUNT)
        PARSE_UINT32(CursedCount,  CORE_MS_TAG_CURSEDCOUNT)
      #undef PARSE_UINT32

      #define PARSE_INT(_property, _tag) \
        { \
          QDomElement elem = e_user.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else { \
            int value = elem.text().toInt(&ok); \
            if (!ok) \
              DOUT("Invalid " #_property); \
            else \
              user.set##_property(value); \
          } \
        }

        PARSE_INT(Score, CORE_MS_TAG_SCORE)
        PARSE_INT(Level, CORE_MS_TAG_LEVEL)
      #undef PARSE_INT

      #define PARSE_BYTEARRAY(_property, _tag) \
        { \
          QDomElement elem = e_user.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else \
            user.set##_property(elem.text().toLatin1()); \
        }

        PARSE_BYTEARRAY(Name,      CORE_MS_TAG_NAME)
        PARSE_BYTEARRAY(Email,     CORE_MS_TAG_EMAIL)
      #undef PARSE_BYTEARRAY

      #define PARSE_STRING(_property, _tag) \
        { \
          QDomElement elem = e_user.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else \
            user.set##_property(elem.text()); \
        }

        PARSE_STRING(NickName,  CORE_MS_TAG_NICKNAME)
      #undef PARSE_STRING

      */

      users.push_back(user);
    }

    return users;
  }

  Universe::MediaTokenList
  MediaServerConnection::parseTokens(const QDomDocument &doc)
  {
    MediaTokenList tokens;
    if (doc.isNull()) {
      DOUT("Invalid tokens document root");
      return tokens;
    }

    QDomNodeList n_tokens = doc.elementsByTagName(CORE_MS_TAG_TOKEN);
    if (n_tokens.size() <= 0) {
      DOUT("No tokens found");
      return tokens;
    }

    for (int j = 0; j < n_tokens.size(); j++) {
      QDomElement e_token = n_tokens.at(j).toElement();
      if (e_token.isNull()) {
        DOUT("Invalid token element");
        continue;
      }

      MediaToken token;

      // <id>
      {
        QString s_id = e_token.attribute(CORE_MS_ATTRIBUTE_ID);
        if (s_id.isNull()) {
          DOUT("Invalid tokenId");
          continue; // Continue for invalidId
        }
        bool ok;
        qint64 id = s_id.toLongLong(&ok);
        if (!ok) {
           DOUT("Invalid token ID attribute:" << s_id);
           continue;
        }
        token.setId(id);
      }

      QDomNode node = e_token.firstChild();
      while (!node.isNull()) {
        QDomElement element = node.toElement();
        QString tagName = element.tagName();

        if (tagName.isEmpty())
          continue;

      #define ELIF_LONGLONG(_property, _tag) \
        else if (tagName == _tag) { \
          bool ok; \
          qint64 value = element.text().toLongLong(&ok); \
          if (!ok) \
            DOUT("Invalid " #_property); \
          else \
            token.set##_property(value); \
        }

        ELIF_LONGLONG(CreateTime, CORE_MS_TAG_CREATETIME)

      #undef ELIF_LONGLONG

      #define ELIF_UINT32(_property, _tag) \
        else if (tagName == _tag) { \
          bool ok; \
          uint value = element.text().toUInt(&ok); \
          if (!ok) \
            DOUT("Invalid " #_property); \
          else \
            token.set##_property((quint32)value); \
        }

       ELIF_UINT32(BlessedCount, CORE_MS_TAG_BLESSEDCOUNT)
       ELIF_UINT32(CursedCount,  CORE_MS_TAG_CURSEDCOUNT)
       ELIF_UINT32(VisitedCount, CORE_MS_TAG_VISITEDCOUNT)
      #undef ELIF_UINT32

      #define ELIF_INT(_property, _tag) \
        else if (tagName == _tag) { \
          bool ok; \
          int value = element.text().toInt(&ok); \
          if (!ok) \
            DOUT("Invalid " #_property); \
          else \
            token.set##_property(value); \
        }

        ELIF_INT(Score, CORE_MS_TAG_SCORE)
      #undef ELIF_INT

      #define ELIF_STRING(_property, _tag) \
        else if (tagName == _tag) { \
          token.set##_property(element.text()); \
        }

        ELIF_STRING(Digest, CORE_MS_TAG_DIGEST)
      #undef ELIF_STRING

        else
          DOUT("parseTokens: Unprocessed tagName:" << tagName);

        node = node.nextSibling();
      }

      /*
      #define PARSE_LONGLONG(_property, _tag) \
        { \
          QDomElement elem = e_token.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else { \
            qint64 value = elem.text().toLongLong(&ok); \
            if (!ok) \
              DOUT("Invalid " #_property); \
            else \
              token.set##_property(value); \
          } \
        }

        PARSE_LONGLONG(CreateTime, CORE_MS_TAG_CREATETIME)
      #undef PARSE_LONGLONG

      #define PARSE_UINT32(_property, _tag) \
        { \
          QDomElement elem = e_token.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else { \
            uint value = elem.text().toUInt(&ok); \
            if (!ok) \
              DOUT("Invalid " #_property); \
            else \
              token.set##_property((quint32)value); \
          } \
        }

        PARSE_UINT32(BlessedCount, CORE_MS_TAG_BLESSEDCOUNT)
        PARSE_UINT32(CursedCount,  CORE_MS_TAG_CURSEDCOUNT)
        PARSE_UINT32(VisitedCount, CORE_MS_TAG_VISITEDCOUNT)
      #undef PARSE_UINT32

      #define PARSE_INT(_property, _tag) \
        { \
          QDomElement elem = e_token.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else { \
            int value = elem.text().toInt(&ok); \
            if (!ok) \
              DOUT("Invalid " #_property); \
            else \
              token.set##_property(value); \
          } \
        }

        PARSE_INT(Score, CORE_MS_TAG_SCORE)
      #undef PARSE_INT
      */

      tokens.push_back(token);
    }

    return tokens;
  }

  Universe::MediaAnnotationList
  MediaServerConnection::parseAnnotations(const QDomDocument &doc)
  {
    MediaAnnotationList annots;
    if (doc.isNull()) {
      DOUT("Invalid annotation document root");
      return annots;
    }

    QDomNodeList n_annots = doc.elementsByTagName(CORE_MS_TAG_ANNOT);
    if (n_annots.size() <= 0) {
      DOUT("No annotations found");
      return annots;
    }

    for (int j = 0; j < n_annots.size(); j++) {
      QDomElement e_annot = n_annots.at(j).toElement();
      if (e_annot.isNull()) {
        DOUT("Invalid annotation element");
        continue;
      }

      MediaAnnotation annot;

      // <id>
      {
        QString s_id = e_annot.attribute(CORE_MS_ATTRIBUTE_ID);
        if (s_id.isNull()) {
          DOUT("Invalid annotId");
          continue; // Continue for invalidId
        }
        bool ok;
        qint64 id = s_id.toLongLong(&ok);
        if (!ok) {
           DOUT("Invalid annotation ID attribute:" << s_id);
           continue;
        }
        annot.setId(id);
      }

      QDomNode node = e_annot.firstChild();
      while (!node.isNull()) {
        QDomElement element = node.toElement();
        QString tagName = element.tagName();

        if (tagName.isEmpty())
          continue;

      #define ELIF_LONGLONG(_property, _tag) \
        else if (tagName == _tag) { \
          bool ok; \
          qint64 value = element.text().toLongLong(&ok); \
          if (!ok) \
            DOUT("Invalid " #_property); \
          else \
            annot.set##_property(value); \
        }

        ELIF_LONGLONG(TokenId,   CORE_MS_TAG_TOKENID)
        ELIF_LONGLONG(UserId,    CORE_MS_TAG_USERID)
        ELIF_LONGLONG(Pos,       CORE_MS_TAG_POS)
        ELIF_LONGLONG(CreateTime, CORE_MS_TAG_CREATETIME)
        ELIF_LONGLONG(UpdateTime, CORE_MS_TAG_UPDATETIME)
      #undef ELIF_LONGLONG

      #define ELIF_UINT32(_property, _tag) \
        else if (tagName == _tag) { \
          bool ok; \
          uint value = element.text().toUInt(&ok); \
          if (!ok) \
            DOUT("Invalid " #_property); \
          else \
            annot.set##_property((quint32)value); \
        }

        ELIF_UINT32(Languages, CORE_MS_TAG_LANGUAGE)

        ELIF_UINT32(BlessedCount, CORE_MS_TAG_BLESSEDCOUNT)
        ELIF_UINT32(CursedCount,  CORE_MS_TAG_CURSEDCOUNT)
        ELIF_UINT32(KilledCount,  CORE_MS_TAG_KILLEDCOUNT)
      #undef ELIF_UINT32

      #define ELIF_INT(_property, _tag) \
        else if (tagName == _tag) { \
          bool ok; \
          int value = element.text().toInt(&ok); \
          if (!ok) \
            DOUT("Invalid " #_property); \
          else \
            annot.set##_property(value); \
        }

        ELIF_INT(Score, CORE_MS_TAG_SCORE)
      #undef ELIF_INT

      #define ELIF_STRING(_property, _tag) \
        else if (tagName == _tag) { \
          annot.set##_property(element.text()); \
        }

        ELIF_STRING(Text,  CORE_MS_TAG_TEXT)            // <text>
      #undef ELIF_STRING
        else
          DOUT("parseAnnotations: Unprocessed tagName:" << tagName);


        node = node.nextSibling();
      }

      /*
      #define PARSE_LONGLONG(_property, _tag) \
        { \
          QDomElement elem = e_annot.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else { \
            qint64 value = elem.text().toLongLong(&ok); \
            if (!ok) \
              DOUT("Invalid " #_property); \
            else \
              annot.set##_property(value); \
          } \
        }

        PARSE_LONGLONG(TokenId,   CORE_MS_TAG_TOKENID)
        PARSE_LONGLONG(UserId,    CORE_MS_TAG_USERID)
        PARSE_LONGLONG(Pos,       CORE_MS_TAG_POS)
        PARSE_LONGLONG(CreateTime, CORE_MS_TAG_CREATETIME)
        PARSE_LONGLONG(UpdateTime, CORE_MS_TAG_UPDATETIME)
      #undef PARSE_LONGLONG

      #define PARSE_UINT32(_property, _tag) \
        { \
          QDomElement elem = e_annot.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else { \
            uint value = elem.text().toUInt(&ok); \
            if (!ok) \
              DOUT("Invalid " #_property); \
            else \
              annot.set##_property((quint32)value); \
          } \
        }

        PARSE_UINT32(Languages, CORE_MS_TAG_LANGUAGE)

        PARSE_UINT32(BlessedCount, CORE_MS_TAG_BLESSEDCOUNT)
        PARSE_UINT32(CursedCount,  CORE_MS_TAG_CURSEDCOUNT)
        PARSE_UINT32(KilledCount,  CORE_MS_TAG_KILLEDCOUNT)
      #undef PARSE_UINT32

      #define PARSE_INT(_property, _tag) \
        { \
          QDomElement elem = e_annot.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else { \
            int value = elem.text().toInt(&ok); \
            if (!ok) \
              DOUT("Invalid " #_property); \
            else \
              annot.set##_property(value); \
          } \
        }

        PARSE_INT(Score, CORE_MS_TAG_SCORE)
      #undef PARSE_INT

      #define PARSE_STRING(_property, _tag) \
        { \
          QDomElement elem = e_annot.firstChildElement(_tag); \
          if (elem.isNull()) \
            DOUT("Missing " #_property); \
          else \
            annot.set##_property(elem.text()); \
        }

        PARSE_STRING(Text,  CORE_MS_TAG_TEXT)            // <text>
      #undef PARSE_STRING
      */

      if (!annot.hasPos())
        annot.setPos(0); // Still display invalid annots in the start.

      annots.push_back(annot);
    }

    return annots;
  }


} // namespace Core

// EOF
