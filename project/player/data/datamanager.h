#ifndef DATAMANAGER_H
#define DATAMANAGER_H

// datamanager.h
// 10/24/2011

#include "core/data/token.h"
#include "core/data/annotation.h"
#include "core/data/alias.h"
#include <QObject>

class Database;
class ServerAgent;

class DataManager : public QObject
{
  Q_OBJECT
  typedef DataManager Self;
  typedef QObject Base;

  typedef Core::Data::Token Token;
  typedef Core::Data::TokenList TokenList;
  typedef Core::Data::Alias Alias;
  typedef Core::Data::AliasList AliasList;
  typedef Core::Data::Annotation Annotation;
  typedef Core::Data::AnnotationList AnnotationList;

public:
  DataManager(ServerAgent *server, Database *cache, Database *queue, QObject *parent = 0);

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
  bool updateAnnotationTextWithId(const QString &text, qint64 id, int tt);

  // - Queries -
public:
  Token selectTokenWithId(qint64 id, int tt);
  Token selectTokenWithDigest(const QString &digest, int tt);

  AnnotationList selectAnnotationsWithTokenId(qint64 tid, int tt);
  AliasList selectAliasesWithTokenId(qint64 tid, int tt);
  AnnotationList selectRelatedAnnotationsWithTokenId(qint64 tid, int tt);

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

#endif // DATAMANAGER_H
