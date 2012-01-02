#ifndef DATAMANAGER_H
#define DATAMANAGER_H
// datamanager.h
// 12/5/2011

#include "core/cloud/alias.h"
#include "core/cloud/annotation.h"
#include "core/cloud/token.h"
#include "core/cloud/user.h"
#include <QObject>

class DataManager : public QObject
{
  Q_OBJECT
  typedef DataManager Self;
  typedef QObject Base;

  typedef Core::Cloud::User User;
  typedef Core::Cloud::Token Token;
  typedef Core::Cloud::Alias Alias;
  typedef Core::Cloud::AliasList AliasList;
  typedef Core::Cloud::Annotation Annotation;
  typedef Core::Cloud::AnnotationList AnnotationList;

  User user_;
  Token token_;
  AliasList aliases_;
  AnnotationList annots_;

public:
  explicit DataManager(QObject *parent = 0) : Base(parent) { }

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

public slots:
  void setUser(const User &user);
  void setToken(const Token &token);
  void addAlias(const Alias &alias);
  void setAliases(const AliasList &aliases);
  void updateAlias(const Alias &alias);
  void setAnnotations(const AnnotationList &annots);
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
  void invalidateAnnotations() { emit annotationsChanged(annots_); }

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
