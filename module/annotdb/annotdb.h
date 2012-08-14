#ifndef ANNOTDB_H
#define ANNOTDB_H

// annotdb.h
// 9/12/2011

#include "module/annotcloud/user.h"
#include "module/annotcloud/token.h"
#include "module/annotcloud/alias.h"
#include "module/annotcloud/annotation.h"
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtSql/QSqlDatabase>

/**
 *  Represents cached database.
 *
 *  Conventions:
 *  - Duplication in inserted value is not checked.
 */
class AnnotationDatabase : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(AnnotationDatabase)
  typedef AnnotationDatabase Self;
  typedef QObject Base;

  QSqlDatabase db_;
  mutable QMutex m_;
  bool disposed_;

  typedef AnnotCloud::User User;
  typedef AnnotCloud::UserList UserList;
  typedef AnnotCloud::Token Token;
  typedef AnnotCloud::TokenList TokenList;
  typedef AnnotCloud::Alias Alias;
  typedef AnnotCloud::AliasList AliasList;
  typedef AnnotCloud::Annotation Annotation;
  typedef AnnotCloud::AnnotationList AnnotationList;

  // - Construction -
public:
  explicit AnnotationDatabase(QObject *parent = nullptr)
    : Base(parent), disposed_(false) { }
  ~AnnotationDatabase();

  bool open(const QString &fileName);
  bool isValid() const { return db_.isOpen(); }
  bool isDisposed() const { return disposed_; }

  ///  Remove all contents saved in the database.
signals:
  void cleared();

public slots:
  void clear();

  void dispose() { disposed_ = true; }

protected:
  bool createTables(); ///< Format current db by creating needed tables
  void tune();
  static QString newId();

  // - Queries -
public:
  qint64 insertUser(const User& user);
  bool insertUsers(const UserList& l);
  qint64 insertToken(const Token &token);
  bool insertTokens(const TokenList &l);
  qint64 insertAlias(const Alias &alias);
  bool isAliasExists(const Alias &alias) const;
  bool insertAliases(const AliasList &l);
  qint64 insertAnnotation(const Annotation &annot, bool lock = true);
  bool insertAnnotations(const AnnotationList &l);

  /// Password is encrypted. Return a user with zero id if failed.
  User selectUserWithNameAndPassword(const QString &name, const QString &password) const;
  User selectUserWithId(qint64 id) const;
  UserList selectUsers() const;
  Token selectTokenWithId(qint64 id) const;
  Token selectTokenWithDigest(const QString &digest, qint32 section) const;
  TokenList selectTokens() const;
  Alias selectAliasWithId(qint64 id) const;
  AliasList selectAliasesWithTokenId(qint64 tid) const;
  AliasList selectAliasesWithTokenDigest(const QString &digest, qint32 section);
  AliasList selectAliases() const;
  Annotation selectAnnotationWithId(qint64 id) const;
  AnnotationList selectAnnotationsWithTokenId(qint64 tid) const;
  AnnotationList selectAnnotationsWithTokenDigest(const QString &digest, qint32 section) const;
  AnnotationList selectAnnotations() const;

  ///  Return -1 if not annotation id not exist
  qint64 selectAliasUpdateTimeWithId(qint64 id) const;
  qint64 selectAnnotationUpdateTimeWithId(qint64 id) const;

  // - Update -
public:
  bool deleteUserWithId(qint64 id);
  void deleteUsers();
  bool deleteTokenWithId(qint64 id);
  void deleteTokenWithDigest(const QString &digest, qint32 section);
  void deleteTokens();
  bool deleteAliasWithId(qint64 id);
  void deleteAliasesWithTokenId(qint64 tid);
  void deleteAliasesWithTokenDigest(const QString &digest, qint32 section);
  void deleteAliases();
  bool deleteAnnotationWithId(qint64 id);
  void deleteAnnotationsWithTokenId(qint64 tid);
  void deleteAnnotationsWithTokenDigest(const QString &digest, qint32 section);
  void deleteAnnotations();

  qint64 updateUser(const User &u)
  { deleteUserWithId(u.id()); return insertUser(u); }

  qint64 updateToken(const Token &t)
  { deleteTokenWithId(t.id()); return insertToken(t); }

  ///  Insert annotation if not exist, or replace the old one. Return updated aid.
  void updateAnnotation(const Annotation &annot);
  void updateAnnotations(const AnnotationList &l, bool async = false, int limit = 0);

  ///  Insert alias if not exist, or replace the old one. Return updated aid.
  void updateAlias(const Alias &alias);
  void updateAliases(const AliasList &l, bool async = false, int limit = 0);

  // - Modification -

  bool updateAnnotationTextWithId(const QString &text, qint64 id);
  bool updateAnnotationUserIdWithId(qint64 userId, qint64 id);
};

#endif // ANNOTDB_H
