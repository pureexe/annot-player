#ifndef SERVERPROXY_H
#define SERVERPROXY_H

// module/serveragent/serverproxy.h
// 9/10/2011

#include "serversoap.h"
#include "core/cloud/user.h"
#include "core/cloud/token.h"
#include "core/cloud/alias.h"
#include "core/cloud/annotation.h"
#include <boost/scoped_ptr.hpp>
#include <QObject>
#include <QMutex>

class ServerProxy: public QObject
{
  Q_OBJECT
  typedef ServerProxy Self;
  typedef QObject Base;

  typedef Core::Cloud::User User;
  typedef Core::Cloud::Token Token;
  typedef Core::Cloud::TokenList TokenList;
  typedef Core::Cloud::Alias Alias;
  typedef Core::Cloud::AliasList AliasList;
  typedef Core::Cloud::Annotation Annotation;
  typedef Core::Cloud::AnnotationList AnnotationList;

  QMutex mutex_;
  boost::scoped_ptr<ServerSoap::ServerSoapProxy> proxy_;

public:
  explicit ServerProxy(QObject *parent = 0);

signals:
  void soapError(int err);

public slots:
  void reset();

  // - Login -
public:
  bool login(const QString &userName, const QString &passwordDigest);
  bool logout();
  QString chat(const QString &message);

  bool getAuthorized();
  bool getConnected();
  User getUser();

  bool setClientType(const QString &typeString);

  // - User -
  bool setUserAnonymous(bool t);
  bool setUserLanguage(qint32 language);

  // - Callback -
public:
  bool setCallback(int port, long key);

  // - Submissions -
public:
  qint64 submitMediaToken(const Token &token);
  qint64 submitGameToken(const Token &token);

  qint64 submitMediaTokenDigest(const QString &digest);
  qint64 submitGameTokenDigest(const QString &digest);

  qint64 submitMediaAlias(const Alias &alias);
  qint64 submitGameAlias(const Alias &alias);

  qint64 submitMediaAliasTextWithTokenId(const QString &text, qint32 aliasType, qint64 tokenId);
  qint64 submitGameAliasTextWithTokenId(const QString &text, qint32 aliasType, qint64 tokenId);

  qint64 submitMediaAliasTextAndTokenDigest(const QString &text, qint32 aliasType, const QString &digest);
  qint64 submitGameAliasTextAndTokenDigest(const QString &text, qint32 aliasType, const QString &digest);

  qint64 submitMediaAnnotation(const Annotation &annot);
  qint64 submitGameAnnotation(const Annotation &annot);

  qint64 submitMediaAnnotationTextWithTokenId(const QString &text, qint64 pos, qint32 posType, qint64 tokenId);
  qint64 submitGameAnnotationTextWithTokenId(const QString &text, qint64 pos, qint32 posType, qint64 tokenId);

  qint64 submitMediaAnnotationTextAndTokenDigest(const QString &text, qint64 pos, qint32 posType, const QString &digest);
  qint64 submitGameAnnotationTextAndTokenDigest(const QString &text, qint64 pos, qint32 posType, const QString &digest);

  // - Update -
public:
  bool updateMediaAnnotationTextWithId(const QString &text, qint64 id);
  bool updateGameAnnotationTextWithId(const QString &text, qint64 id);

  // - Queries -
public:
  Token selectMediaTokenWithId(qint64 id);
  Token selectGameTokenWithId(qint64 id);
  Token selectMediaTokenWithDigest(const QString &digest);
  Token selectGameTokenWithDigest(const QString &digest);

  AnnotationList selectMediaAnnotationsWithTokenId(qint64 tid);
  AnnotationList selectGameAnnotationsWithTokenId(qint64 tid);
  AnnotationList selectRelatedMediaAnnotationsWithTokenId(qint64 tid);
  AnnotationList selectRelatedGameAnnotationsWithTokenId(qint64 tid);

  AliasList selectMediaAliasesWithTokenId(qint64 tid);
  AliasList selectGameAliasesWithTokenId(qint64 tid);

  // - Cast -
public:
  bool blessMediaAnnotationWithId(qint64 id);
  bool blessGameAnnotationWithId(qint64 id);
};

#endif // SERVERPROXY_H
