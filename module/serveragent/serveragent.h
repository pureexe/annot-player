#ifndef SERVERAGENT_H
#define SERVERAGENT_H

// module/serveragent/serveragent.h
// 9/10/2011
// Connect server side ServerAgentService.
// See: http://blog.csdn.net/tingsking18/article/details/5456831
//
// Note: since soap timeout is not slow, synchronized approach is used.
// TODO: Deal with offline case here??

#include <QObject>
#include "core/data/user.h"
#include "core/data/token.h"
#include "core/data/alias.h"
#include "core/data/annotation.h"

class ServerProxy;
class ClientAgent;

class ServerAgent: public QObject
{
  Q_OBJECT
  typedef ServerAgent Self;
  typedef QObject Base;

  typedef Core::Data::User User;
  typedef Core::Data::Token Token;
  typedef Core::Data::TokenList TokenList;
  typedef Core::Data::Alias Alias;
  typedef Core::Data::AliasList AliasList;
  typedef Core::Data::Annotation Annotation;
  typedef Core::Data::AnnotationList AnnotationList;

  ServerProxy *proxy_;
  User user_;
  bool authorized_;
  bool connected_;

#ifdef USE_MODULE_CLIENTAGENT
  ClientAgent *client_;
#endif // USE_MODULE_CLIENTAGENT

public:
  explicit ServerAgent(QObject *parent = 0);

#ifdef USE_MODULE_CLIENTAGENT
  void setClientAgent(ClientAgent *client);
#endif // USE_MODULE_CLIENTAGENT

  // - Login -
public:
  bool isAuthorized() const; ///< If is logged in as an authorized user
  bool isConnected() const;

  const User &user() const; ///< Logged in user
  User &user(); ///< Used for hacking offline mode
  void setUser(const User &user); ///< \internal

  QString chat(const QString &message);

signals:
  void userChanged();

  void loginRequested(const QString &userName);
  void loginSucceeded(const QString &userName);
  void loginFailed(const QString &userName);

  void logoutRequested();
  void logoutFinished();

  // Error signals:
  void connected(); ///< succeeded
  void connectionError();
  void error404();
  void serverError();
  void unknownError();

  // - Connections -
public slots:
  bool login(const QString &userName, const QString &passwordDigest);
  void login(); // Login using last userName and password
  void logout();

  void setConnected(bool yes = true); ///< forcely set connection flag
  void setAuthorized(bool yes = true); ///< forcely set authorization flag
  void updateConnected();
  void updateAuthorized();

  void disconnect(); ///< set connected to false

  void reportSoapError(int err); ///< \internal

  bool isSoftwareUpdated();

  // - User -
public:
  bool setUserAnonymous(bool t = true);
  bool setUserLanguage(qint32 language);

  // - Submissions -
public:
  qint64 submitToken(const Token &token);
  bool submitTokens(const TokenList &tokens);
  qint64 submitAlias(const Alias &alias);
  bool submitAliases(const AliasList &tokens);
  qint64 submitTokenAndAlias(const Token &token, const Alias &alias); ///< return token id only

  qint64 submitAnnotation(const Annotation &annot);
  bool submitAnnotations(const AnnotationList &annots);

  // - Update -
public:
  bool updateAnnotationTextWithId(const QString &text, qint64 id, int tt);

  // - Queries -
public:
  Token selectTokenWithId(qint64 id, int tt);
  Token selectTokenWithDigest(const QString &digest, int tt);

  AnnotationList selectAnnotationsWithTokenId(qint64 tid, int tt);
  AliasList selectAliasesWithTokenId(qint64 tid, int tt);

 ///  Clustered at server based on token's aliases
  AnnotationList selectRelatedAnnotationsWithTokenId(qint64 tid, int tt);

  // - Cast -
public:
  bool blessAnnotationWithId(qint64 id, int tt);
};

#endif // SERVERAGENT_H
