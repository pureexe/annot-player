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

  // - Submission -
public:
  qint64 submitToken(const Token &token, const QString &userName, const QString &password);
  qint64 submitTokenDigest(const QString &digest, qint32 digestType, qint32 tokenType, const QString &userName, const QString &password);

  qint64 submitAlias(const Alias &alias, const QString &userName, const QString &password);
  qint64 submitAliasTextWithTokenId(const QString &text, qint32 type, qint64 tokenId, const QString &userName, const QString &password);
  qint64 submitAliasTextAndTokenDigest(const QString &text, qint32 type, const QString &digest, qint32 digestType, const QString &userName, const QString &password);

  qint64 submitAnnotation(const Annotation &annot, const QString &userName, const QString &password);
  qint64 submitAnnotationTextWithTokenId(const QString &text, qint64 pos, qint32 posType, qint64 tokenId, const QString &userName, const QString &password);
  qint64 submitAnnotationTextAndTokenDigest(const QString &text, qint64 pos, qint32 posType, const QString &digest, qint32 digestType, const QString &userName, const QString &password);

  // - Deletion -
public:
  bool deleteAnnotationWithId(qint64 id, const QString &userName, const QString &password);
  bool deleteAliasWithId(qint64 id, const QString &userName, const QString &password);

  // - Update -
public:
  bool updateAnnotationTextWithId(const QString &text, qint64 id, const QString &userName, const QString &password);

  // - Queries -
public:
  Token selectTokenWithId(qint64 id);
  Token selectTokenWithDigest(const QString &digest, qint32 digestType);

  AnnotationList selectAnnotationsWithTokenId(qint64 tid);
  AnnotationList selectRelatedAnnotationsWithTokenId(qint64 tid);

  AliasList selectAliasesWithTokenId(qint64 tid);

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

  bool blockUserWithId(qint64 id, const QString &userName, const QString &password);
};

#endif // SERVERPROXY_H
