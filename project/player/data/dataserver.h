#ifndef DATASERVER_H
#define DATASERVER_H

// dataserver.h
// 10/24/2011

#include "module/annotcloud/token.h"
#include "module/annotcloud/annotation.h"
#include "module/annotcloud/alias.h"
#include <QObject>

class Database;
class ServerAgent;
class SignalHub;

class DataServer : public QObject
{
  Q_OBJECT
  typedef DataServer Self;
  typedef QObject Base;

  typedef AnnotCloud::Token Token;
  typedef AnnotCloud::TokenList TokenList;
  typedef AnnotCloud::Alias Alias;
  typedef AnnotCloud::AliasList AliasList;
  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

public:
  DataServer(SignalHub *hub, ServerAgent *server, Database *cache, Database *queue, QObject *parent = 0)
    : Base(parent), hub_(hub), server_(server), cache_(cache), queue_(queue) { }

public slots:
  void dispose();

  // - Submission -
public:
  qint64 submitToken(const Token &token);
  bool submitTokens(const TokenList &tokens);
  qint64 submitAlias(const Alias &alias);
  bool submitAliases(const AliasList &aliases);
  qint64 submitTokenAndAlias(const Token &token, const Alias &alias); ///< return token id only
  qint64 submitTokenAndAliases(const Token &token, const AliasList &l); ///< return token id only

  qint64 submitAnnotation(const Annotation &annot);
  bool submitAnnotations(const AnnotationList &annots);

  // - Update -
public slots:
  bool updateAnnotationTextWithId(const QString &text, qint64 id);

  void updateAliases(const AliasList &l);
  void updateAnnotations(const AnnotationList &l);

  // - Deletion -
public slots:
  bool deleteAlias(const Alias &alias);
  bool deleteAliasWithId(qint64 id);
  bool deleteAnnotation(const Annotation &annot);
  bool deleteAnnotationWithId(qint64 id);

  // - Queries -
public:
  Token selectTokenWithId(qint64 id);
  Token selectTokenWithDigest(const QString &digest, qint32 part);

  AnnotationList selectAnnotationsWithTokenId(qint64 tid);
  AliasList selectAliasesWithTokenId(qint64 tid);
  AnnotationList selectRelatedAnnotationsWithTokenId(qint64 tid);
  AliasList selectRelatedAliasesWithTokenId(qint64 tid);

  AliasList selectAliasesWithToken(const Token &token);
  AnnotationList selectAnnotationsWithToken(const Token &token);

  // - Sync -
public:
  bool commitQueue();

  // - Implementations -
private:
  SignalHub *hub_;
  ServerAgent *server_;
  Database *cache_,
           *queue_;
};

#endif // DATASERVER_H
