#ifndef DB_H
#define DB_H

// db.h
// 9/12/2011

#include "core/data/user.h"
#include "core/data/token.h"
#include "core/data/alias.h"
#include "core/data/annotation.h"
#include <QObject>
#include <QSqlDatabase>

/**
 *  Represents cached database.
 *
 *  Conventions:
 *  - Duplication in inserted value is not checked.
 */
class Database : public QObject
{
  Q_OBJECT
  typedef Database Self;
  typedef QObject Base;

  QSqlDatabase db_;

  typedef Core::Data::User User;
  typedef Core::Data::UserList UserList;
  typedef Core::Data::Token Token;
  typedef Core::Data::TokenList TokenList;
  typedef Core::Data::Alias Alias;
  typedef Core::Data::AliasList AliasList;
  typedef Core::Data::Annotation Annotation;
  typedef Core::Data::AnnotationList AnnotationList;

  // - Constructions -
public:
  explicit Database(const QString &filePath, QObject *parent = 0);
  ~Database();

  bool isValid() const;

  ///  Remove all contents saved in the database.
signals:
  void cleared();

public slots:
  void clear();

protected:
  const QSqlDatabase &db() const;
  bool open(const QString &filePath);
  bool createTables(); ///< Format current db by creating needed tables

  // - Queries -
public:
  qint64 insertUser(const User& user);
  bool insertUsers(const UserList& l);
  qint64 insertToken(const Token &token);
  bool insertTokens(const TokenList &l);
  qint64 insertAlias(const Alias &alias);
  bool isAliasExists(const Alias &alias) const;
  bool insertAliases(const AliasList &l);
  qint64 insertAnnotation(const Annotation &annot);
  bool insertAnnotations(const AnnotationList &l);

  /// Password is encrypted. Return a user with zero id if failed.
  User selectUserWithNameAndPassword(const QString &name, const QString &password) const;
  User selectUserWithId(qint64 id) const;
  UserList selectUsers() const;
  Token selectTokenWithId(qint64 id, int tt) const;
  Token selectTokenWithDigest(const QString &digest) const;
  TokenList selectTokens() const;
  Alias selectAliasWithId(qint64 id, int tt) const;
  AliasList selectAliasesWithTokenId(qint64 tid, int tt) const;
  AliasList selectAliasesWithTokenDigest(const QString &digest);
  AliasList selectAliases() const;
  Annotation selectAnnotationWithId(qint64 id, int tt) const;
  AnnotationList selectAnnotationsWithTokenId(qint64 tid, int tt) const;
  AnnotationList selectAnnotationsWithTokenDigest(const QString &digest) const;
  AnnotationList selectAnnotations() const;

  void deleteUserWithId(qint64 id);
  void deleteUsers();
  void deleteTokenWithId(qint64 id, int tt);
  void deleteTokenWithDigest(const QString &digest);
  void deleteTokens();
  void deleteAliasWithId(qint64 id, int tt);
  void deleteAliasesWithTokenId(qint64 tid, int tt);
  void deleteAliasesWithTokenDigest(const QString &digest);
  void deleteAliases();
  void deleteAnnotationWithId(qint64 id, int tt);
  void deleteAnnotationsWithTokenId(qint64 tid, int tt);
  void deleteAnnotationsWithTokenDigest(const QString &digest);
  void deleteAnnotations();
};

#endif // DB_H
