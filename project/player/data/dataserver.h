#ifndef DATASERVER_H
#define DATASERVER_H

// dataserver.h
// 10/24/2011

#include "module/annotcloud/token.h"
#include "module/annotcloud/annotation.h"
#include "module/annotcloud/alias.h"
#include <QtCore/QObject>

class AnnotationDatabase;
class AnnotationServerAgent;
class SignalHub;

class DataServer : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(DataServer)
  typedef DataServer Self;
  typedef QObject Base;

  typedef AnnotCloud::Token Token;
  typedef AnnotCloud::TokenList TokenList;
  typedef AnnotCloud::Alias Alias;
  typedef AnnotCloud::AliasList AliasList;
  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

public:
  DataServer(SignalHub *hub, AnnotationServerAgent *server,
      AnnotationDatabase *cache, AnnotationDatabase *queue, QObject *parent = nullptr)
    : Base(parent), preferCache_(false), hub_(hub), server_(server), cache_(cache), queue_(queue) { }

  bool preferCache() const { return preferCache_; }

signals:
  void preferCacheChanged(bool t);
  void message(QString text);
  void warning(QString text);

public slots:
  void dispose();
  void setPreferCache(bool t) { preferCache_ = t; emit preferCacheChanged(preferCache_); }

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
  bool updateAnnotationUserIdWithId(qint64 userId, qint64 id);

  //void updateAliases(const AliasList &l);
  //void updateAnnotations(const AnnotationList &l);

  // - Deletion -
public slots:
  bool deleteAlias(const Alias &alias);
  bool deleteAliasWithId(qint64 id);
  bool deleteAnnotation(const Annotation &annot);
  bool deleteAnnotationWithId(qint64 id);

  // - Queries -
public:
  Token selectTokenWithId(qint64 id);
  Token selectTokenWithDigest(const QString &digest, qint32 section);

  AnnotationList selectAnnotationsWithTokenId(qint64 tid, bool ignoreCache = false);
  AliasList selectAliasesWithTokenId(qint64 tid, bool ignoreCache = false, bool *fromCache = nullptr);
  AnnotationList selectRelatedAnnotationsWithTokenId(qint64 tid, bool ignoreCache = false, bool *fromCache = nullptr);
  AliasList selectRelatedAliasesWithTokenId(qint64 tid);

  AliasList selectAliasesWithToken(const Token &token, bool ignoreCache = false, bool *fromCache = nullptr);
  AnnotationList selectAnnotationsWithToken(const Token &token, bool ignoreCache = false, bool *fromCache = nullptr);

  // - Sync -
public:
  bool commitQueue();

  // - Implementations -
private:
  bool preferCache_;
  SignalHub *hub_;
  AnnotationServerAgent *server_;
  AnnotationDatabase *cache_,
                     *queue_;
};

#endif // DATASERVER_H
