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

  // - User -
public:

  bool getConnected();

  User selectUser(const QString &userName, const QString &password);
  bool setUserAnonymous(bool t, const QString &userName, const QString &password);
  bool setUserLanguage(qint32 language, const QString &userName, const QString &password);

  bool isClientUpdated(const QString &clientVersion);

  QString chat(const QString &message);

  // - Submissions -
public:
  qint64 submitMediaToken(const Token &token, const QString &userName, const QString &password);
  qint64 submitGameToken(const Token &token, const QString &userName, const QString &password);

  qint64 submitMediaTokenDigest(const QString &digest, const QString &userName, const QString &password);
  qint64 submitGameTokenDigest(const QString &digest, const QString &userName, const QString &password);

  qint64 submitMediaAlias(const Alias &alias, const QString &userName, const QString &password);
  qint64 submitGameAlias(const Alias &alias, const QString &userName, const QString &password);

  qint64 submitMediaAliasTextWithTokenId(const QString &text, qint32 aliasType, qint64 tokenId, const QString &userName, const QString &password);
  qint64 submitGameAliasTextWithTokenId(const QString &text, qint32 aliasType, qint64 tokenId, const QString &userName, const QString &password);

  qint64 submitMediaAliasTextAndTokenDigest(const QString &text, qint32 aliasType, const QString &digest, const QString &userName, const QString &password);
  qint64 submitGameAliasTextAndTokenDigest(const QString &text, qint32 aliasType, const QString &digest, const QString &userName, const QString &password);

  qint64 submitMediaAnnotation(const Annotation &annot, const QString &userName, const QString &password);
  qint64 submitGameAnnotation(const Annotation &annot, const QString &userName, const QString &password);

  qint64 submitMediaAnnotationTextWithTokenId(const QString &text, qint64 pos, qint32 posType, qint64 tokenId, const QString &userName, const QString &password);
  qint64 submitGameAnnotationTextWithTokenId(const QString &text, qint64 pos, qint32 posType, qint64 tokenId, const QString &userName, const QString &password);

  qint64 submitMediaAnnotationTextAndTokenDigest(const QString &text, qint64 pos, qint32 posType, const QString &digest, const QString &userName, const QString &password);
  qint64 submitGameAnnotationTextAndTokenDigest(const QString &text, qint64 pos, qint32 posType, const QString &digest, const QString &userName, const QString &password);

  // - Update -
public:
  bool updateMediaAnnotationTextWithId(const QString &text, qint64 id, const QString &userName, const QString &password);
  bool updateGameAnnotationTextWithId(const QString &text, qint64 id, const QString &userName, const QString &password);

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
  bool blessMediaAnnotationWithId(qint64 id, const QString &userName, const QString &password);
  bool blessGameAnnotationWithId(qint64 id, const QString &userName, const QString &password);
};

#endif // SERVERPROXY_H
