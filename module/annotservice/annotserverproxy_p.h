#ifndef ANNOTSERVERPROXY_H
#define ANNOTSERVERPROXY_H

// annotserverproxy.h
// 9/10/2011

#include "module/annotservice/cloudservicesoap_p.h"
#include "module/annotcloud/alias.h"
#include "module/annotcloud/annotation.h"
#include "module/annotcloud/gamethread.h"
#include "module/annotcloud/token.h"
#include "module/annotcloud/user.h"
#include <QtCore/QMutex>

class AnnotationServerProxy: public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationServerProxy)
  typedef AnnotationServerProxy Self;
  typedef QObject Base;

  typedef AnnotCloud::User User;
  typedef AnnotCloud::Token Token;
  typedef AnnotCloud::TokenList TokenList;
  typedef AnnotCloud::Alias Alias;
  typedef AnnotCloud::AliasList AliasList;
  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;
  typedef AnnotCloud::GameThread GameThread;
  typedef AnnotCloud::GameThreadList GameThreadList;

  QMutex mutex_;
  CloudServiceProxy *proxy_;

public:
  explicit AnnotationServerProxy(QObject *parent = nullptr);
  ~AnnotationServerProxy();

signals:
  void soapError(int err);

  // - User -
public:
  bool login(const QString &userName, const QString &password);
  User getUser();

  bool getConnected();

  bool setUserAnonymous(bool t);
  bool setUserLanguage(qint32 language);

  bool isLatestApp(const QString &clientVersion);

  // - Submission -
public:
  qint64 submitToken(const Token &token);
  qint64 submitTokenDigest(const QString &digest, qint32 section, qint32 tokenType);
  qint64 submitTokenUrl(const QString &url, qint32 section, qint32 tokenType);

  qint64 submitAlias(const Alias &alias);
  qint64 submitAliasTextWithTokenId(const QString &text, qint32 type, qint64 tokenId);
  qint64 submitAliasTextAndTokenDigest(const QString &text, qint32 type, const QString &digest, qint32 section);

  qint64 submitAnnotation(const Annotation &annot);
  qint64 submitAnnotationTextWithTokenId(const QString &text, qint64 pos, qint32 posType, qint64 tokenId);
  qint64 submitAnnotationTextAndTokenDigest(const QString &text, qint64 pos, qint32 posType, const QString &digest, qint32 section);

  qint64 submitGameThread(const GameThread &thread);
  // - Deletion -
public:
  bool deleteAnnotationWithId(qint64 id);
  bool deleteAliasWithId(qint64 id);

  // - Update -
public:
  bool updateAnnotationTextWithId(const QString &text, qint64 id);
  bool updateAnnotationUserIdWithId(qint64 userId, qint64 id);
  bool updateGameThread(const GameThread &thread);

  // - Queries -
public:
  qint64 selectTokenIdWithDigest(const QString &digest, qint32 section);
  qint64 selectTokenIdWithUrl(const QString &url, qint32 section);

  Token selectTokenWithId(qint64 id);
  Token selectTokenWithDigest(const QString &digest, qint32 section);

  AnnotationList selectAnnotationsWithTokenId(qint64 tid);
  AnnotationList selectRelatedAnnotationsWithTokenId(qint64 tid);

  AliasList selectAliasesWithTokenId(qint64 tid);
  AliasList selectRelatedAliasesWithTokenId(qint64 tid);

  GameThread selectGameThreadWithTokenId(qint64 tid);
  GameThread selectGameThreadWithTokenDigest(const QString &digest);
  // - Live -
public:
  QString chat(const QString &message);
  /*
  qint64 submitLiveAnnotationTextWithTokenId(const QString &text, qint64 tokenId);
  AnnotationList selectLiveAnnotationsWithTokenId(qint64 tid);
  qint32 selectLiveTokenIntervalWithId(qint64 tid);
  */

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
};

#endif // ANNOTSERVERPROXY_H
