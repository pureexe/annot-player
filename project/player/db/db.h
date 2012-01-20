#ifndef DB_H
#define DB_H

// db.h
// 9/12/2011

#include "core/cloud/user.h"
#include "core/cloud/token.h"
#include "core/cloud/alias.h"
#include "core/cloud/annotation.h"
#include <QObject>
#include <QMutex>
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
  mutable QMutex mutex_;
  bool disposed_;

  typedef Core::Cloud::User User;
  typedef Core::Cloud::UserList UserList;
  typedef Core::Cloud::Token Token;
  typedef Core::Cloud::TokenList TokenList;
  typedef Core::Cloud::Alias Alias;
  typedef Core::Cloud::AliasList AliasList;
  typedef Core::Cloud::Annotation Annotation;
  typedef Core::Cloud::AnnotationList AnnotationList;

  // - Construction -
public:
  explicit Database(const QString &filePath, QObject *parent = 0);
  ~Database();

  bool isValid() const { return db_.isOpen(); }
  bool isDisposed() const { return disposed_; }

  ///  Remove all contents saved in the database.
signals:
  void cleared();

public slots:
  void clear();

  void dispose();

protected:
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
  Token selectTokenWithId(qint64 id) const;
  Token selectTokenWithDigest(const QString &digest, qint32 digestType) const;
  TokenList selectTokens() const;
  Alias selectAliasWithId(qint64 id) const;
  AliasList selectAliasesWithTokenId(qint64 tid) const;
  AliasList selectAliasesWithTokenDigest(const QString &digest, qint32 digestType);
  AliasList selectAliases() const;
  Annotation selectAnnotationWithId(qint64 id) const;
  AnnotationList selectAnnotationsWithTokenId(qint64 tid) const;
  AnnotationList selectAnnotationsWithTokenDigest(const QString &digest, qint32 digestType) const;
  AnnotationList selectAnnotations() const;

  ///  Return -1 if not annotation id not exist
  qint64 selectAnnotationUpdateTimeWithId(qint64 id) const;

  // - Update -
public:
  bool deleteUserWithId(qint64 id);
  void deleteUsers();
  bool deleteTokenWithId(qint64 id);
  void deleteTokenWithDigest(const QString &digest, qint32 digestType);
  void deleteTokens();
  bool deleteAliasWithId(qint64 id);
  void deleteAliasesWithTokenId(qint64 tid);
  void deleteAliasesWithTokenDigest(const QString &digest, qint32 digestType);
  void deleteAliases();
  bool deleteAnnotationWithId(qint64 id);
  void deleteAnnotationsWithTokenId(qint64 tid);
  void deleteAnnotationsWithTokenDigest(const QString &digest, qint32 digestType);
  void deleteAnnotations();

  qint64 updateUser(const User &u)
  { deleteUserWithId(u.id()); return insertUser(u); }

  qint64 updateToken(const Token &t)
  { deleteTokenWithId(t.id()); return insertToken(t); }

  qint64 updateAlias(const Alias &a)
  { deleteAliasWithId(a.id()); return insertAlias(a); }

  ///  Insert annotation if not exist, or replace the old one. Return updated aid.
  void updateAnnotation(const Annotation &annot);
  void updateAnnotations(const AnnotationList &l, bool async = false);
};

#endif // DB_H
