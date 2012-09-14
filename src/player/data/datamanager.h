#ifndef DATAMANAGER_H
#define DATAMANAGER_H
// datamanager.h
// 12/5/2011

#include "lib/annotcloud/alias.h"
#include "lib/annotcloud/annotation.h"
#include "lib/annotcloud/gamehook.h"
#include "lib/annotcloud/gamethread.h"
#include "lib/annotcloud/token.h"
#include "lib/annotcloud/user.h"
#include <QtCore/QObject>
#include <QtCore/QHash>

class DataManager : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(DataManager)
  typedef DataManager Self;
  typedef QObject Base;

  typedef AnnotCloud::User User;
  typedef AnnotCloud::Token Token;
  typedef AnnotCloud::Alias Alias;
  typedef AnnotCloud::AliasList AliasList;
  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;
  typedef AnnotCloud::GameHook GameHook;
  typedef AnnotCloud::GameHookList GameHookList;
  typedef AnnotCloud::GameThread GameThread;
  typedef AnnotCloud::GameThreadList GameThreadList;

  User user_;
  Token token_;
  AliasList aliases_;
  AnnotationList annots_;

  GameHook hook_;
  GameThread thread_;

  mutable QStringList urls_; // annotation urls

  // Statistics
  QHash<qint64, int> userAnnotCount_;
  qint64 minAnnotTime_, maxAnnotTime_;

public:
  explicit DataManager(QObject *parent = nullptr);

  // - Properties -
public:
  const User &user() const { return user_; }
  User &user() { return user_; }
  bool hasUser() const { return user_.isValid(); }

  const Token &token() const { return token_; }
  Token &token() { return token_; }
  bool hasToken() const { return token_.isValid(); }

  const AliasList &aliases() const { return aliases_; }
  bool hasAliases() const { return !aliases_.isEmpty(); }

  const AnnotationList &annotations() const { return annots_; }
  bool hasAnnotations() const { return !annots_.isEmpty(); }

  bool aliasConflicts(const Alias &a) const;
  bool urlConflicts(const QString &url) const;

  static QString normalizeUrl(const QString &url);

  int userCount() const { return userAnnotCount_.size(); }
  int annotationCountForUserId(qint64 uid) const;
  qint64 minAnnotationCreateTime() const { return minAnnotTime_; }
  qint64 maxAnnotationCreateTime() const { return maxAnnotTime_; }

  const QStringList &urls() const
  { if (urls_.isEmpty()) updateUrls(); return urls_; }

  const GameThread &gameThread() const { return thread_; }
  GameThread &gameThread() { return thread_; }
  bool hasGameThread() const { return thread_.isValid(); }

  const GameHook &gameHook() const { return hook_; }
  GameHook &gameHook() { return hook_; }
  bool hasGameHook() const { return hook_.isValid(); }

public slots:
  void setUser(const User &user) { user_ = user; invalidateUser(); }
  void setToken(const Token &token) { token_ = token; invalidateToken(); }
  void clearToken() { setToken(Token()); }
  void addAlias(const Alias &alias) { aliases_.append(alias); invalidateUrls(); emit aliasAdded(alias); }
  void setAliases(const AliasList &aliases) { aliases_ = aliases; invalidateAliases(); }
  void updateAlias(const Alias &alias);
  void setAnnotations(const AnnotationList &l) { annots_ = l; invalidateAnnotations(); }
  void addAnnotation(const Annotation &annot);
  void updateAnnotation(const Annotation &annot);
  void setGameThread(const GameThread &thread) { thread_ = thread; }
  void setGameHook(const GameHook &hook) { hook_ = hook; }

  void updateAnnotationTextWithId(const QString &text, qint64 id);
  void updateAnnotationUserIdWithId(qint64 userId, qint64 id);

  void removeAnnotationWithId(qint64 id);
  void removeAliasWithId(qint64 id);

  void removeUser() { setUser(User()); }
  void removeToken() { setToken(Token()); }
  void removeAliases() { setAliases(AliasList()); }
  void removeAnnotations() { setAnnotations(AnnotationList()); }
  void removeGameThread() { setGameThread(GameThread()); }
  void removeGameHook() { setGameHook(GameHook()); }
  //void clear() { removeToken(); removeAliases(); removeAnnotations(); removeGameThread(); }

  void invalidateUser() { emit userChanged(user_); }
  void invalidateToken() { invalidateUrls(); emit tokenChanged(token_); }
  void invalidateAliases() { invalidateUrls(); emit aliasesChanged(aliases_); }
  void invalidateAnnotations();

protected:
  void invalidateUrls() const { if (!urls_.isEmpty()) urls_.clear(); }
  void updateUrls() const;

signals:
  void userChanged(User user);
  void tokenChanged(Token token);
  void aliasesChanged(AliasList aliases);
  void aliasAdded(Alias alias);
  void aliasUpdated(Alias alias);
  void aliasRemoved(Alias);
  void aliasRemovedWithId(qint64 id);
  void annotationsChanged(AnnotationList annots);
  void annotationAdded(Annotation annot);
  void annotationUpdated(Annotation annot);
  void annotationRemovedWithId(qint64 id);
  void annotationTextUpdatedWithId(const QString &text, qint64 id);
  void annotationUserIdUpdatedWithId(qint64 userId, qint64 id);
};

#endif // DATAMANAGER_H
