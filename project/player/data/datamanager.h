#ifndef DATAMANAGER_H
#define DATAMANAGER_H
// datamanager.h
// 12/5/2011

#include "module/annotcloud/alias.h"
#include "module/annotcloud/annotation.h"
#include "module/annotcloud/token.h"
#include "module/annotcloud/user.h"
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

  User user_;
  Token token_;
  AliasList aliases_;
  AnnotationList annots_;

  // Statistics
  QHash<qint64, int> userAnnotCount_;
  qint64 minAnnotTime_, maxAnnotTime_;

public:
  explicit DataManager(QObject *parent = 0);

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

  int userCount() const { return userAnnotCount_.size(); }
  qint64 minAnnotationCreateTime() const { return minAnnotTime_; }
  qint64 maxAnnotationCreateTime() const { return maxAnnotTime_; }

public slots:
  void setUser(const User &user) { user_ = user; invalidateUser(); }
  void setToken(const Token &token) { token_ = token; invalidateToken(); }
  void addAlias(const Alias &alias) { aliases_.append(alias); emit aliasAdded(alias); }
  void setAliases(const AliasList &aliases) { aliases_ = aliases; invalidateAliases(); }
  void updateAlias(const Alias &alias);
  void setAnnotations(const AnnotationList &l) { annots_ = l; invalidateAnnotations(); }
  void addAnnotation(const Annotation &annot);
  void updateAnnotation(const Annotation &annot);

  void updateAnnotationTextWithId(const QString &text, qint64 id);

  void removeAnnotationWithId(qint64 id);
  void removeAliasWithId(qint64 id);

  void removeUser() { setUser(User()); }
  void removeToken() { setToken(Token()); }
  void removeAliases() { setAliases(AliasList()); }
  void removeAnnotations() { setAnnotations(AnnotationList()); }
  void clear() { removeToken(); removeAliases(); removeAnnotations(); }

  void invalidateUser() { emit userChanged(user_); }
  void invalidateToken() { emit tokenChanged(token_); }
  void invalidateAliases() { emit aliasesChanged(aliases_); }
  void invalidateAnnotations();

signals:
  void userChanged(User user);
  void tokenChanged(Token token);
  void aliasesChanged(AliasList aliases);
  void aliasAdded(Alias alias);
  void aliasUpdated(Alias alias);
  void aliasRemovedWithId(qint64 id);
  void annotationsChanged(AnnotationList annots);
  void annotationAdded(Annotation annot);
  void annotationUpdated(Annotation annot);
  void annotationRemovedWithId(qint64 id);
  void annotationTextUpdatedWithId(const QString &text, qint64 id);
};

#endif // DATAMANAGER_H
