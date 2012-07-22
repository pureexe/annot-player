#ifndef SERVERPROXY_H
#define SERVERPROXY_H

// module/serveragent/serverproxy.h
// 9/10/2011

#include "module/serveragent/serversoap.h"
#include "module/annotcloud/alias.h"
#include "module/annotcloud/annotation.h"
#include "module/annotcloud/token.h"
#include "module/annotcloud/user.h"
#include <QtCore/QMutex>
#include <boost/scoped_ptr.hpp>

class ServerProxy: public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(ServerProxy)
  typedef ServerProxy Self;
  typedef QObject Base;

  typedef AnnotCloud::User User;
  typedef AnnotCloud::Token Token;
  typedef AnnotCloud::TokenList TokenList;
  typedef AnnotCloud::Alias Alias;
  typedef AnnotCloud::AliasList AliasList;
  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

  QMutex mutex_;
  boost::scoped_ptr<ServerSoap::ServerSoapProxy> proxy_;

public:
  explicit ServerProxy(QObject *parent = nullptr)
    : Base(parent) { reset(); }

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

  // - Submission -
public:
  qint64 submitToken(const Token &token, const QString &userName, const QString &password);
  qint64 submitTokenDigest(const QString &digest, qint32 part, qint32 tokenType, const QString &userName, const QString &password);
  qint64 submitTokenSource(const QString &source, qint32 part, qint32 tokenType, const QString &userName, const QString &password);

  qint64 submitAlias(const Alias &alias, const QString &userName, const QString &password);
  qint64 submitAliasTextWithTokenId(const QString &text, qint32 type, qint64 tokenId, const QString &userName, const QString &password);
  qint64 submitAliasTextAndTokenDigest(const QString &text, qint32 type, const QString &digest, qint32 part, const QString &userName, const QString &password);

  qint64 submitAnnotation(const Annotation &annot, const QString &userName, const QString &password);
  qint64 submitAnnotationTextWithTokenId(const QString &text, qint64 pos, qint32 posType, qint64 tokenId, const QString &userName, const QString &password);
  qint64 submitAnnotationTextAndTokenDigest(const QString &text, qint64 pos, qint32 posType, const QString &digest, qint32 part, const QString &userName, const QString &password);

  // - Deletion -
public:
  bool deleteAnnotationWithId(qint64 id, const QString &userName, const QString &password);
  bool deleteAliasWithId(qint64 id, const QString &userName, const QString &password);

  // - Update -
public:
  bool updateAnnotationTextWithId(const QString &text, qint64 id, const QString &userName, const QString &password);
  bool updateAnnotationUserIdWithId(qint64 userId, qint64 id, const QString &userName, const QString &password);

  // - Queries -
public:
  qint64 selectTokenIdWithDigest(const QString &digest, qint32 part);
  qint64 selectTokenIdWithSource(const QString &source, qint32 part);

  Token selectTokenWithId(qint64 id);
  Token selectTokenWithDigest(const QString &digest, qint32 part);

  AnnotationList selectAnnotationsWithTokenId(qint64 tid);
  AnnotationList selectRelatedAnnotationsWithTokenId(qint64 tid);

  AliasList selectAliasesWithTokenId(qint64 tid);
  AliasList selectRelatedAliasesWithTokenId(qint64 tid);

  // - Live -
public:
  QString chat(const QString &message);

  qint64 submitLiveAnnotationTextWithTokenId(const QString &text, qint64 tokenId, const QString &userName, const QString &password);

  AnnotationList selectLiveAnnotationsWithTokenId(qint64 tid);

  qint32 selectLiveTokenIntervalWithId(qint64 tid);

  // - Cast -
public:
  bool blessAnnotationWithId(qint64 id, const QString &userName, const QString &password);
  bool curseAnnotationWithId(qint64 id, const QString &userName, const QString &password);
  bool blockAnnotationWithId(qint64 id, const QString &userName, const QString &password);

  bool blessAliasWithId(qint64 id, const QString &userName, const QString &password);
  bool curseAliasWithId(qint64 id, const QString &userName, const QString &password);
  bool blockAliasWithId(qint64 id, const QString &userName, const QString &password);

  bool blessTokenWithId(qint64 id, const QString &userName, const QString &password);
  bool curseTokenWithId(qint64 id, const QString &userName, const QString &password);

  bool blessUserWithId(qint64 id, const QString &userName, const QString &password);
  bool blockUserWithId(qint64 id, const QString &userName, const QString &password);
  bool curseUserWithId(qint64 id, const QString &userName, const QString &password);
};

#endif // SERVERPROXY_H
