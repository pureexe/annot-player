#ifndef DATASERVER_H
#define DATASERVER_H

// dataserver.h
// 10/24/2011

#include "core/cloud/token.h"
#include "core/cloud/annotation.h"
#include "core/cloud/alias.h"
#include <QObject>

class Database;
class ServerAgent;

class DataServer : public QObject
{
  Q_OBJECT
  typedef DataServer Self;
  typedef QObject Base;

  typedef Core::Cloud::Token Token;
  typedef Core::Cloud::TokenList TokenList;
  typedef Core::Cloud::Alias Alias;
  typedef Core::Cloud::AliasList AliasList;
  typedef Core::Cloud::Annotation Annotation;
  typedef Core::Cloud::AnnotationList AnnotationList;

public:
  DataServer(ServerAgent *server, Database *cache, Database *queue, QObject *parent = 0);

  // - Submission -
public:
  qint64 submitToken(const Token &token);
  bool submitTokens(const TokenList &tokens);
  qint64 submitAlias(const Alias &alias);
  bool submitAliases(const AliasList &aliases);
  qint64 submitTokenAndAlias(const Token &token, const Alias &alias); ///< return token id only

  qint64 submitAnnotation(const Annotation &annot);
  bool submitAnnotations(const AnnotationList &annots);

  // - Update -
public:
  bool updateAnnotationTextWithId(const QString &text, qint64 id);

  // - Queries -
public:
  Token selectTokenWithId(qint64 id);
  Token selectTokenWithDigest(const QString &digest, qint32 digestType);

  AnnotationList selectAnnotationsWithTokenId(qint64 tid);
  AliasList selectAliasesWithTokenId(qint64 tid);
  AnnotationList selectRelatedAnnotationsWithTokenId(qint64 tid);

  AliasList selectAliasesWithToken(const Token &token);
  AnnotationList selectAnnotationsWithToken(const Token &token);

  // - Sync -
public:
  bool commitQueue();

  // - Implementations -
private:
  ServerAgent *server_;
  Database *cache_;
  Database *queue_;
};

#endif // DATASERVER_H
