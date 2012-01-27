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
#include <QMutex>
#include "core/cloud/user.h"
#include "core/cloud/token.h"
#include "core/cloud/alias.h"
#include "core/cloud/annotation.h"

class ServerProxy;
class ClientAgent;
class CastManager;

class ServerAgent: public QObject
{
  Q_OBJECT
  typedef ServerAgent Self;
  typedef QObject Base;

  typedef Core::Cloud::User User;
  typedef Core::Cloud::Token Token;
  typedef Core::Cloud::TokenList TokenList;
  typedef Core::Cloud::Alias Alias;
  typedef Core::Cloud::AliasList AliasList;
  typedef Core::Cloud::Annotation Annotation;
  typedef Core::Cloud::AnnotationList AnnotationList;

public:
  explicit ServerAgent(QObject *parent = 0);

#ifdef USE_MODULE_CLIENTAGENT
  void setClientAgent(ClientAgent *client);
#endif // USE_MODULE_CLIENTAGENT

  // - Login -
public:
  ///  If is logged in as an authorized user
  bool isAuthorized() const { return authorized_; }
  bool isConnected() const { return connected_; }

  const User &user() const; ///< Logged in user
  User &user(); ///< Used for hacking offline mode
  void setUser(const User &user); ///< \internal

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
  bool updateAnnotationTextWithId(const QString &text, qint64 id);

  // - Queries -
public:
  Token selectTokenWithId(qint64 id);
  Token selectTokenWithDigest(const QString &digest, qint32 part);

  AnnotationList selectAnnotationsWithTokenId(qint64 tid);
  AliasList selectAliasesWithTokenId(qint64 tid);

 ///  Clustered at server based on token's aliases
  AnnotationList selectRelatedAnnotationsWithTokenId(qint64 tid);

  // - Deletion -
public:
  bool deleteAnnotationWithId(qint64 id);
  bool deleteAliasWithId(qint64 id);

  // - Cast -
public:
  bool blessAnnotationWithId(qint64 id);
  bool curseAnnotationWithId(qint64 id);
  bool blockAnnotationWithId(qint64 id);

  bool blessAliasWithId(qint64 id);
  bool curseAliasWithId(qint64 id);
  bool blockAliasWithId(qint64 id);

  bool blessTokenWithId(qint64 id);
  bool curseTokenWithId(qint64 id);

  bool blessUserWithId(qint64 id);
  bool blockUserWithId(qint64 id);
  bool curseUserWithId(qint64 id);

  bool isAnnotationBlessedWithId(qint64 id) const;
  bool isAnnotationCursedWithId(qint64 id) const;
  bool isAnnotationBlockedWithId(qint64 id) const;

  bool isAliasBlessedWithId(qint64 id) const;
  bool isAliasCursedWithId(qint64 id) const;
  bool isAliasBlockedWithId(qint64 id) const;

  bool isTokenBlessedWithId(qint64 id) const;
  bool isTokenCursedWithId(qint64 id) const;

  bool isUserBlockedWithId(qint64 id) const;

  // - Live -
public:
  QString chat(const QString &message);

  qint64 submitLiveAnnotation(const Annotation &annot);
  qint64 submitLiveAnnotationText(const QString &text);

  AnnotationList selectLiveAnnotations();

  qint32 selectLiveTokenInterval();

  // - Implementations -
private:
  QMutex mutex_;
  ServerProxy *proxy_;
  CastManager *cast_;
  User user_;
  bool authorized_;
  bool connected_;

#ifdef USE_MODULE_CLIENTAGENT
  ClientAgent *client_;
#endif // USE_MODULE_CLIENTAGENT

};

#endif // SERVERAGENT_H
