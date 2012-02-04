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

using namespace AnnotCloud;
using namespace Logger;

//#define DEBUG "dataserver"
#include "module/debug/debug.h"

// - Constructions -

void
DataServer::dispose()
{
  cache_->dispose();
  queue_->dispose();
}

// - Submission -

qint64
DataServer::submitToken(const Token &token)
{
  DOUT("enter");
  qint64 id = 0;
  if (server_->isConnected()) {
    if (server_->isAuthorized())
      id = server_->submitToken(token);
    else
      id = server_->selectTokenId(token);
  }
  if (!id && cache_->isValid() && token.hasDigest())
    id = cache_->selectTokenWithDigest(token.digest(), token.part()).id();
  DOUT("exit: ret =" << id);
  return id;
}

bool
DataServer::submitTokens(const TokenList &tokens)
{
  DOUT("exit: count =" << tokens.size());
  bool ret = true;
  if (!tokens.isEmpty())
    foreach (Token t, tokens)
      ret = ret && submitToken(t);
  DOUT("exit: ret =" << ret);
  return ret;
}

qint64
DataServer::submitAlias(const Alias &alias)
{
  DOUT("enter");
  if (!alias.hasText()) {
    DOUT("exit: missing text");
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
  DOUT("exit: id =" << id);
  return id;
}

bool
DataServer::submitAliases(const AliasList &aliases)
{
  DOUT("enter: count =" << aliases.size());
  bool ret = true;
  if (!aliases.isEmpty())
    foreach (Alias a, aliases)
      ret = ret && submitAlias(a);
  DOUT("exit: ret =" << ret);
  return ret;
}

qint64
DataServer::submitTokenAndAlias(const Token &token, const Alias &alias)
{
  DOUT("enter");
  qint64 tid = submitToken(token);
  if (tid && alias.hasText()) {
    Alias a = alias;
    a.setTokenId(tid);;
    submitAlias(a);
  }
  DOUT("exit: ret =" << tid);
  return tid;
}

qint64
DataServer::submitAnnotation(const Annotation &annot)
{
  DOUT("enter");
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
  DOUT("exit: ret =" << id);
  return id;
}

bool
DataServer::submitAnnotations(const AnnotationList &annots)
{
  DOUT("enter: count =" << annots.size());
  bool ret = true;
  if (!annots.isEmpty())
    foreach (Annotation a, annots)
      ret = ret && submitAnnotation(a);
  DOUT("exit: ret =" << ret);
  return ret;
}

// - Update -

bool
DataServer::updateAnnotationTextWithId(const QString &text, qint64 id)
{
  DOUT("enter: text =" << text);
  bool ret = false;
  if (server_->isConnected() && server_->isAuthorized())
    ret = server_->updateAnnotationTextWithId(text, id);
  if (ret)
    log(tr("annotation saved") + ": " + text);
  else
    warn(tr("failed to update annotation text") + ": " + text);
  DOUT("exit: ret =" << ret);
  return ret;
}

// - Deletion -

bool
DataServer::deleteAnnotation(const Annotation &a)
{
  DOUT("enter: text =" << a.text());
  if (!a.hasId()) {
    warn("cannot delete offline annotation");
    DOUT("exit: failed");
    return false;
  }
  if (!a.hasUserId() || a.userId() != server_->user().id()) {
    warn("cannot delete other's annotation");
    DOUT("exit: failed");
    return false;
  }

  bool ret = deleteAnnotationWithId(a.id());
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
DataServer::deleteAnnotationWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  if (!id) {
    DOUT("exit: failed, invalid id 0");
    return false;
  }

  bool ret = true;
  if (server_->isConnected() && server_->isAuthorized())
    ret = server_->deleteAnnotationWithId(id);
  if (cache_->isValid())
    cache_->deleteAnnotationWithId(id);

  if (ret)
    log(tr("annotation deleted"));
  else
    warn(tr("failed to delete annotation"));
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
DataServer::deleteAlias(const Alias &a)
{
  DOUT("enter: text =" << a.text());
  if (!a.hasId()) {
    warn("cannot delete offline alias");
    DOUT("exit: failed");
    return false;
  }
  if (!a.hasUserId() || a.userId() != server_->user().id()) {
    warn("cannot delete other's alias");
    DOUT("exit: failed");
    return false;
  }

  bool ret = deleteAliasWithId(a.id());
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
DataServer::deleteAliasWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  if (!id) {
    DOUT("exit: failed, invalid id 0");
    return false;
  }

  bool ret = true;
  if (server_->isConnected() && server_->isAuthorized())
    ret = server_->deleteAliasWithId(id);

  if (cache_->isValid())
    cache_->deleteAliasWithId(id);

  if (ret)
    log(tr("alias deleted"));
  else
    warn(tr("failed to delete alias"));
  DOUT("exit: ret =" << ret);
  return ret;
}

// - Queries -

Token
DataServer::selectTokenWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Token ret;
  if (server_->isConnected()) {
    ret = server_->selectTokenWithId(id);
    if (ret.isValid() && cache_->isValid()) {
      cache_->deleteTokenWithId(ret.id());
      cache_->insertToken(ret);
    }
  } else if (cache_->isValid())
    ret = cache_->selectTokenWithId(id);
  DOUT("exit: tid =" << ret.id());
  return ret;
}

Token
DataServer::selectTokenWithDigest(const QString &digest, qint32 part)
{
  DOUT("enter: digest =" << digest);
  Token ret;
  if (server_->isConnected()) {
    ret = server_->selectTokenWithDigest(digest, part);
    if (ret.isValid() && cache_->isValid()) {
      cache_->deleteTokenWithId(ret.id());
      cache_->insertToken(ret);
    }
  } else if (cache_->isValid())
    ret = cache_->selectTokenWithDigest(digest, part);
  DOUT("exit: tid =" << ret.id());
  return ret;
}

AnnotationList
DataServer::selectAnnotationsWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  AnnotationList ret;
  if (server_->isConnected()) {
    ret = server_->selectAnnotationsWithTokenId(tid);
    updateAnnotations(ret);
  } else if (cache_->isValid())
    ret = cache_->selectAnnotationsWithTokenId(tid);;
  DOUT("exit: count =" << ret.size());
  return ret;
}

AliasList
DataServer::selectAliasesWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  AliasList ret;
  if (server_->isConnected()) {
    ret = server_->selectAliasesWithTokenId(tid);
    if (!ret.isEmpty() && cache_->isValid()) {
      cache_->deleteAliasesWithTokenId(tid);
      cache_->insertAliases(ret);
    }
  } else if (cache_->isValid())
    ret = cache_->selectAliasesWithTokenId(tid);;
  DOUT("exit: count =" << ret.size());
  return ret;
}

AnnotationList
DataServer::selectRelatedAnnotationsWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  AnnotationList ret;
  if (server_->isConnected()) {
    ret = server_->selectRelatedAnnotationsWithTokenId(tid);
    updateAnnotations(ret);
  } else if (cache_->isValid())
    ret = cache_->selectAnnotationsWithTokenId(tid);
  DOUT("exit: count =" << ret.size());
  return ret;
}

void
DataServer::updateAnnotations(const AnnotationList &l)
{
  if (!l.isEmpty() && cache_->isValid()) {
    if (l.size() > 20) // async for large amount of annots
      cache_->updateAnnotations(l, true); // async = true
    else
      cache_->updateAnnotations(l, false); // async =false
  }
}

AliasList
DataServer::selectAliasesWithToken(const Token &token)
{
  DOUT("enter");
  AliasList ret;
  if (token.isValid())
    ret = selectAliasesWithTokenId(token.id());
  else if (token.hasDigest() && cache_->isValid())
    ret = cache_->selectAliasesWithTokenDigest(token.digest(), token.part());
  DOUT("exit");
  return ret;
}

AnnotationList
DataServer::selectAnnotationsWithToken(const Token &token)
{
  DOUT("enter");
  AnnotationList ret;
  if (token.isValid())
    ret = selectRelatedAnnotationsWithTokenId(token.id());
  else if (token.hasDigest() && cache_->isValid())
    ret = cache_->selectAnnotationsWithTokenDigest(token.digest(), token.part());
  DOUT("exit: count =" << ret.size());
  return ret;
}

// - Sync -

bool
DataServer::commitQueue()
{
  DOUT("enter");
  if (Settings::globalInstance()->isQueueEmpty()) {
    DOUT("exit: no changes, ret =" << true);
    return true;
  }

  if (!server_->isConnected() || !server_->isAuthorized() || !queue_->isValid()) {
    DOUT("exit: not connected or queue corrupted, ret =" << false);
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

  DOUT("exit: ret =" << empty);
  return empty;
}
// EOF
