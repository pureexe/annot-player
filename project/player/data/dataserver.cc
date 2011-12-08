// dataserver.cc
// 10/24/2011

#include "dataserver.h"
#include "db.h"
#include "settings.h"
#include "logger.h"
#ifdef USE_MODULE_SERVERAGENT
  #include "module/serveragent/serveragent.h"
#else
  #error "server agent module is indispensible"
#endif // USE_MODULE_SERVERAGENT
#include <QtCore>

using namespace Core::Cloud;
using namespace Logger;

#define DEBUG "DataServer"
#include "module/debug/debug.h"

// - Constructions -

DataServer::DataServer(ServerAgent *server, Database *cache, Database *queue, QObject *parent)
  : Base(parent), server_(server), cache_(cache), queue_(queue)
{
  Q_ASSERT(server_);
  Q_ASSERT(cache_);
  Q_ASSERT(queue_);
}

// - Submission -

qint64
DataServer::submitToken(const Token &token)
{
  DOUT("submitToken:enter");
  qint64 id = 0;
  if (server_->isConnected() && server_->isAuthorized())
    id = server_->submitToken(token);
  else if (cache_->isValid())
    id = cache_->selectTokenWithDigest(token.digest(), token.digestType()).id();
  DOUT("submitToken:exit: ret =" << id);
  return id;
}

bool
DataServer::submitTokens(const TokenList &tokens)
{
  DOUT("submitTokens:exit: count =" << tokens.size());
  bool ret = true;
  if (!tokens.isEmpty())
    foreach (Token t, tokens)
      ret = ret && submitToken(t);
  DOUT("submitTokens:exit: ret =" << ret);
  return ret;
}

qint64
DataServer::submitAlias(const Alias &alias)
{
  DOUT("submitAlias:enter");
  if (!alias.hasText()) {
    DOUT("submitAlias:exit: missing text");
    return 0;
  }
  qint64 id = 0;
  if (server_->isConnected() && server_->isAuthorized()) {
    id = server_->submitAlias(alias);
  } else if (queue_->isValid() && !queue_->isAliasExists(alias)) {
    queue_->insertAlias(alias);
    Settings::globalInstance()->setQueueEmpty(false);
  }

  if (cache_->isValid() && !cache_->isAliasExists(alias)) {
    if (id) {
      Alias a = alias;
      a.setId(id);
      cache_->insertAlias(a);
    } else
      cache_->insertAlias(alias);
  }
  DOUT("submitAlias:exit: id =" << id);
  return id;
}

bool
DataServer::submitAliases(const AliasList &aliases)
{
  DOUT("submitAliases:enter: count =" << aliases.size());
  bool ret = true;
  if (!aliases.isEmpty())
    foreach (Alias a, aliases)
      ret = ret && submitAlias(a);
  DOUT("submitAliases:exit: ret =" << ret);
  return ret;
}

qint64
DataServer::submitTokenAndAlias(const Token &token, const Alias &alias)
{
  DOUT("submitTokenAndAlias:enter");
  qint64 tid = submitToken(token);
  if (tid && alias.hasText()) {
    Alias a = alias;
    a.setTokenId(tid);;
    submitAlias(a);
  }
  DOUT("submitTokenAndAlias:exit: ret =" << tid);
  return tid;
}

qint64
DataServer::submitAnnotation(const Annotation &annot)
{
  DOUT("submitAnnotation:enter");
  qint64 id = 0;
  if (server_->isConnected() && server_->isAuthorized()) {
    id = server_->submitAnnotation(annot);
  } else if (queue_->isValid()) {
    queue_->insertAnnotation(annot);
    Settings::globalInstance()->setQueueEmpty(false);
  }
  if (cache_->isValid()) {
    if (id) {
      Annotation a = annot;
      a.setId(id);
      cache_->insertAnnotation(a);
    } else
      cache_->insertAnnotation(annot);
  }
  DOUT("submitAnnotation:exit: ret =" << id);
  return id;
}

bool
DataServer::submitAnnotations(const AnnotationList &annots)
{
  DOUT("submitAnnotations:enter: count =" << annots.size());
  bool ret = true;
  if (!annots.isEmpty())
    foreach (Annotation a, annots)
      ret = ret && submitAnnotation(a);
  DOUT("submitAnnotations:exit: ret =" << ret);
  return ret;
}

// - Update -

bool
DataServer::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  DOUT("udpateAnnotationTextWithId:enter: text =" << text);
  bool ret = false;
  if (server_->isConnected() && server_->isAuthorized())
    ret = server_->updateAnnotationTextWithId(text, id);
  DOUT("udpateAnnotationTextWithId:exit: ret =" << ret);
  return ret;
}

// - Queries -

Token
DataServer::selectTokenWithId(qint64 id)
{
  DOUT("selectTokenWithId:enter: id =" << id);
  Token ret;
  if (server_->isConnected()) {
    ret = server_->selectTokenWithId(id);
    if (ret.isValid() && cache_->isValid()) {
      cache_->deleteTokenWithId(ret.id());
      cache_->insertToken(ret);
    }
  } else if (cache_->isValid())
    ret = cache_->selectTokenWithId(id);
  DOUT("selectTokenWithId:exit: tid =" << ret.id());
  return ret;
}

Token
DataServer::selectTokenWithDigest(const QString &digest, qint32 digestType)
{
  DOUT("selectTokenWithDigest:enter: digest =" << digest);
  Token ret;
  if (server_->isConnected()) {
    ret = server_->selectTokenWithDigest(digest, digestType);
    if (ret.isValid() && cache_->isValid()) {
      cache_->deleteTokenWithId(ret.id());
      cache_->insertToken(ret);
    }
  } else if (cache_->isValid())
    ret = cache_->selectTokenWithDigest(digest, digestType);
  DOUT("selectTokenWithDigest:exit: tid =" << ret.id());
  return ret;
}

AnnotationList
DataServer::selectAnnotationsWithTokenId(qint64 tid)
{
  DOUT("selectAnnotationsWithTokenId:enter: tid =" << tid);
  AnnotationList ret;
  if (server_->isConnected()) {
    ret = server_->selectAnnotationsWithTokenId(tid);
    if (!ret.isEmpty() && cache_->isValid()) {
      cache_->deleteAnnotationsWithTokenId(tid);
      cache_->insertAnnotations(ret);
    }
  } else if (cache_->isValid())
    ret = cache_->selectAnnotationsWithTokenId(tid);;
  DOUT("selectAnnotationsWithTokenId:exit: count =" << ret.size());
  return ret;
}

AliasList
DataServer::selectAliasesWithTokenId(qint64 tid)
{
  DOUT("selectAliasesWithTokenId:enter: tid =" << tid);
  AliasList ret;
  if (server_->isConnected()) {
    ret = server_->selectAliasesWithTokenId(tid);
    if (!ret.isEmpty() && cache_->isValid()) {
      cache_->deleteAliasesWithTokenId(tid);
      cache_->insertAliases(ret);
    }
  } else if (cache_->isValid())
    ret = cache_->selectAliasesWithTokenId(tid);;
  DOUT("selectAliasesWithTokenId:exit: count =" << ret.size());
  return ret;
}

AnnotationList
DataServer::selectRelatedAnnotationsWithTokenId(qint64 tid)
{
  DOUT("selectRelatedAnnotationsWithTokenId:enter: tid =" << tid);
  AnnotationList ret;
  if (server_->isConnected()) {
    ret = server_->selectRelatedAnnotationsWithTokenId(tid);
    if (!ret.isEmpty() && cache_->isValid())
      foreach (Annotation a, ret) {
        cache_->deleteAnnotationWithId(a.id());
        cache_->insertAnnotation(a);
      }
  } else if (cache_->isValid())
    ret = cache_->selectAnnotationsWithTokenId(tid);
  DOUT("selectRelatedAnnotationsWithTokenId:exit: count =" << ret.size());
  return ret;
}

AliasList
DataServer::selectAliasesWithToken(const Token &token)
{
  DOUT("selectAliasesWithToken:enter");
  AliasList ret;
  if (token.isValid())
    ret = selectAliasesWithTokenId(token.id());
  else if (token.hasDigest() && cache_->isValid())
    ret = cache_->selectAliasesWithTokenDigest(token.digest(), token.digestType());
  DOUT("selectAliasesWithToken:exit");
  return ret;
}

AnnotationList
DataServer::selectAnnotationsWithToken(const Token &token)
{
  DOUT("selectAnnotationsWithToken:enter");
  AnnotationList ret;
  if (token.isValid())
    ret = selectAnnotationsWithTokenId(token.id());
  else if (token.hasDigest() && cache_->isValid())
    ret = cache_->selectAnnotationsWithTokenDigest(token.digest(), token.digestType());
  DOUT("selectAnnotationsWithToken:exit: count =" << ret.size());
  return ret;
}

// - Sync -

bool
DataServer::commitQueue()
{
  DOUT("commitQueue:enter");
  if (!Settings::globalInstance()->isQueueEmpty()) {
    DOUT("commitQueue:exit: no changes, ret =" << true);
    return true;
  }

  if (!server_->isConnected() || !server_->isAuthorized() || !queue_->isValid()) {
    DOUT("commitQueue:exit: not connected or queue corrupted, ret =" << false);
    return false;
  }

  bool empty = true;

  AnnotationList annots = queue_->selectAnnotations();
  if (!annots.empty()) {
    bool ok = server_->submitAnnotations(annots);
    if (ok)
      queue_->deleteAnnotations();
    else {
      warn(tr("failed to commit offline annotations")); // TODO: singleShot try again later
      empty = false;
    }
  }

  AliasList aliases = queue_->selectAliases();
  if (!aliases.empty()) {
    bool ok = server_->submitAliases(aliases);
    if (ok)
      queue_->deleteAliases();
    else {
      warn(tr("failed to commit offline aliases")); // TODO: singleShot try again later
      empty = false;
    }
  }

  if (empty)
    Settings::globalInstance()->setQueueEmpty(true);

  DOUT("commitQueue:exit: ret =" << empty);
  return empty;
}
// EOF
