// db.cc
// 9/12/2011
//
// Note 9/13/2011: SQLite doesn't allow insert multple rows at the same time.
// The way to bypass this is ugly and might not be more efficient.
//
// SQLite functions:
// - date and time: http://www.sqlite.org/lang_datefunc.html
// - hex: http://www.sqlite.org/lang_corefunc.html
//   unhex: not available

#include "db.h"
#include "db_config.h"
#include <QtCore>
#include <QtSql>

//#define DEBUG "database"
#include "module/debug/debug.h"

using namespace Core::Cloud;

namespace { // anonymous, helper

  inline QString uniqueString() { static int count = 0; return QString::number(++count); }

} // anonymous namespace

// - Constructions -

Database::Database(const QString &fileName, QObject *parent)
  : Base(parent)
{
  DOUT("enter: fileName =" << fileName);
  QFileInfo fi(fileName);
  bool empty = !fi.exists() || fi.size() == 0;

  db_ = QSqlDatabase::addDatabase("QSQLITE", uniqueString());
  if (db_.isValid())
    open(fileName);
  if (isValid() && empty) {
    bool ok = createTables();
    if (!ok) {
      DOUT("failed to create database tables");
      db_.close();
      DOUT("try to remove corrupted db:" << fileName);
      QFile::remove(fileName);
    }
  }

  if (db_.isValid())
    Q_ASSERT(db_.driver() && db_.driver()->hasFeature(QSqlDriver::LastInsertId));
  DOUT("exit: valid =" << isValid());
}

Database::~Database()
{
  if (db_.isOpen())
    db_.close();
  //db_.removeDatabase(db_.connectionName());
}

bool
Database::isValid() const
{ return db_.isOpen(); }

bool
Database::open(const QString &fileName)
{
  DOUT("fileName =" << fileName);
  Q_ASSERT(!db_.isOpen());
  if (db_.isOpen())
    db_.close();
  db_.setDatabaseName(fileName);
  return db_.open();
}

const QSqlDatabase&
Database::db() const
{ return db_; }

void
Database::clear()
{
  DOUT("enter");
  if (db_.isOpen()) {
    db_.close();
  }
  db_.removeDatabase(db_.connectionName());

  QString fileName = db_.databaseName();
  if (QFileInfo(fileName).exists()) {
    DOUT("try to remove old db file:" << fileName);
    QFile::remove(fileName);
  }
  db_ = QSqlDatabase::addDatabase("QSQLITE", uniqueString());
  if (db_.isValid())
    open(fileName);
  if (isValid()) {
    bool ok = createTables();
    if (!ok) {
      DOUT("failed to create database tables");
      db_.close();
      DOUT("try to remove corrupted db:" << fileName);
      QFile::remove(fileName);
    }
  } else
    DOUT("failed to reset database");

  emit cleared();
  DOUT("exit");
}

// - Create -

bool
Database::createTables()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);

  DOUT("creating table 'user' ...");
  bool ok = query.exec(DB_CREATE_TABLE_USER);
  if (!ok) {
    DOUT("exit: failed to create 'user', error:" << query.lastError());
    return false;
  } else
    DOUT("succeeded");

  DOUT("creating table 'token' ...");
  ok = query.exec(DB_CREATE_TABLE_TOKEN);
  if (!ok) {
    DOUT("exit: failed to create 'token', error:" << query.lastError());
    return false;
  } else
    DOUT("succeeded");

  DOUT("creating table 'alias' ...");
  ok = query.exec(DB_CREATE_TABLE_ALIAS);
  if (!ok) {
    DOUT("exit: failed to create 'token_alias', error:" << query.lastError());
    return false;
  } else
    DOUT("succeeded");

  DOUT("creating table 'annot' ...");
  ok = query.exec(DB_CREATE_TABLE_ANNOT);
  if (!ok) {
    DOUT("exit: failed to create 'annot', error:" << query.lastError());
    return false;
  } else
    DOUT("exit: succeeded");

  return true;
}

// - Insertion -

qint64
Database::insertUser(const User& user)
{
  DOUT("enter: username =" << user.name() << ", id =" << user.id());
  Q_ASSERT(isValid());
  QSqlQuery query(db_);

  DB_INSERT_USER(user, query);

  bool ok = query.exec();
  qint64 ret = 0;
  if (ok)
    ret = query.lastInsertId().toLongLong(&ok);

  if (!ok) {
    DOUT("error:" << query.lastError());
    ret = 0;
  }

  DOUT("exit: id =" << ret);
  return ret;
}

bool
Database::insertUsers(const UserList &l)
{
  DOUT("enter: count =" << l.size());
  Q_ASSERT(isValid());
  bool ret = true;
  foreach (User u, l)
    if (!insertUser(u)) {
      DOUT("failed at id =" << u.id() << ", ignored");
      ret = false;
    }
  DOUT("exit: ret =" << ret);
  return ret;
}


qint64
Database::insertToken(const Token &token)
{
  DOUT("enter: id =" << token.id());
  Q_ASSERT(isValid());
  QSqlQuery query(db_);

  DB_INSERT_TOKEN(token, query);

  bool ok = query.exec();
  qint64 ret = 0;
  if (ok)
    ret = query.lastInsertId().toLongLong(&ok);

  if (!ok) {
    DOUT("error:" << query.lastError());
    ret = 0;
  }

  DOUT("exit: id =" << ret);
  return ret;
}

bool
Database::insertTokens(const TokenList &l)
{
  DOUT("enter: count =" << l.size());
  Q_ASSERT(isValid());
  bool ret = true;
  foreach (Token t, l)
    if (!insertToken(t)) {
      DOUT("failed at id =" << t.id() << ", ignored");
      ret = false;
    }
  DOUT("exit: ret =" << ret);
  return ret;
}

qint64
Database::insertAlias(const Alias &alias)
{
  DOUT("enter: id =" << alias.id()
       << ", tid =" << alias.tokenId()
       << ", uid =" << alias.userId()
       << ", text =" << alias.text());
  Q_ASSERT(isValid());
  QSqlQuery query(db_);

  DB_INSERT_ALIAS(alias, query);

  bool ok = query.exec();
  qint64 ret = 0;
  if (ok)
    ret = query.lastInsertId().toLongLong(&ok);

  if (!ok) {
    DOUT("error:" << query.lastError());
    ret = 0;
  }

  DOUT("exit: ret =" << ret);
  return ret;
}

bool
Database::isAliasExists(const Alias &alias) const
{
  DOUT("enter: id =" << alias.id()
       << ", tid =" << alias.tokenId()
       << ", uid =" << alias.userId()
       << ", text =" << alias.text());
  Q_ASSERT(isValid());
  if (!alias.hasTokenId() && alias.hasTokenDigest()) {
    DOUT("exit: missing token id and digest");
    return false;
  }

  QSqlQuery query(db_);
  if (alias.hasTokenId()) {
    DOUT("query using token_id");
    query.prepare("SELECT alias_id FROM alias WHERE token_id = ? AND alias_type = ? AND alias_text LIKE ?");
    query.addBindValue(alias.tokenId());
    query.addBindValue(alias.type());
    query.addBindValue(alias.text());
  } else if (alias.hasTokenDigest()){
    DOUT("query using token_digest");
    query.prepare("SELECT alias_id FROM alias WHERE token_digest ? AND alias_type = ? AND alias_text LIKE ?");
    query.addBindValue(alias.tokenDigest());
    query.addBindValue(alias.type());
    query.addBindValue(alias.text());
  }

  if (!query.exec()) {
    DOUT("error:" << query.lastError());
    return false;
  }

  bool ret = query.size();
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
Database::insertAliases(const AliasList &l)
{
  DOUT("enter: count =" << l.size());
  Q_ASSERT(isValid());
  bool ret = true;
  foreach (Alias a, l)
    if (!isAliasExists(a))
      if (!insertAlias(a)) {
        DOUT("failed at id =" << a.id() << ", ignored");
        ret = false;
      }
  DOUT("exit: ret =" << ret);
  return ret;
}


qint64
Database::insertAnnotation(const Annotation &annot)
{
  DOUT("enter: id =" << annot.id()
       << ", tid =" << annot.tokenId()
       << ", uid =" << annot.userId());
  Q_ASSERT(isValid());
  QSqlQuery query(db_);

  DB_INSERT_ANNOT(annot, query);

  bool ok = query.exec();
  qint64 ret = 0;
  if (ok)
    ret = query.lastInsertId().toLongLong(&ok);

  if (!ok) {
    DOUT("error:" << query.lastError());
    ret = 0;
  }

  DOUT("exit: ret =" << ret);
  return ret;
}

bool
Database::insertAnnotations(const AnnotationList &l)
{
  DOUT("enter: count =" << l.size());
  Q_ASSERT(isValid());
  bool ret = true;
  foreach (Annotation a, l)
    if (!insertAnnotation(a)) {
      DOUT("failed at id =" << a.id() << ", ignored");
      ret = false;
    }
  DOUT("exit: ret =" << ret);
  return ret;
}

// - Selection -

User
Database::selectUserWithNameAndPassword(const QString &name, const QString &password) const
{
  DOUT("enter: name =" << name << ", password =" << password);
  Q_ASSERT(isValid());
  User ret;

  QSqlQuery query(db_);
  query.prepare(DB_SELECT_USER "WHERE user_name = ? AND user_password = ?");
  query.addBindValue(name);     // 1:user_name
  query.addBindValue(password); // 2:user_password

  bool ok = query.exec();
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }
  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  DB_SET_USER(ret, query);

  DOUT("exit: userid =" << ret.id());
  return ret;
}

User
Database::selectUserWithId(qint64 id) const
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  //Q_ASSERT(id);
  User ret;

  QSqlQuery query(db_);
  query.prepare(DB_SELECT_USER "WHERE user_id = ?");
  query.addBindValue(id);

  bool ok = query.exec();
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }
  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  DB_SET_USER(ret, query);

  DOUT("exit: username =" << ret.name());
  return ret;
}

UserList
Database::selectUsers() const
{
  DOUT("enter");
  Q_ASSERT(isValid());
  UserList ret;

  QSqlQuery query(db_);
  bool ok = query.exec(DB_SELECT_USER);

  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (query.next()) {
    User user;
    DB_SET_USER(user, query);
    ret.append(user);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}


Token
Database::selectTokenWithId(qint64 id) const
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  //Q_ASSERT(id);
  Token ret;

  QSqlQuery query(db_);

  query.prepare(DB_SELECT_TOKEN "WHERE token_id = ?");
  query.addBindValue(id);

  bool ok = query.exec();
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }
  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  DB_SET_TOKEN(ret, query);

  DOUT("exit: digest =" << ret.digest());
  return ret;
}

Token
Database::selectTokenWithDigest(const QString &digest, qint32 digestType) const
{
  DOUT("enter: digestType =" << digestType << ", digest =" << digest);
  Q_ASSERT(isValid());
  //if (digest.isEmpty())
  //  return;
  Token ret;

  QSqlQuery query(db_);

  query.prepare(DB_SELECT_TOKEN "WHERE token_digest = ? AND token_digest_type = ?");
  query.addBindValue(digest);
  query.addBindValue(digestType);

  bool ok = query.exec();
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }
  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  DB_SET_TOKEN(ret, query);

  DOUT("exit: id =" << ret.id());
  return ret;
}

TokenList
Database::selectTokens() const
{
  DOUT("enter");
  Q_ASSERT(isValid());
  TokenList ret;

  QSqlQuery query(db_);
  bool ok = query.exec(DB_SELECT_TOKEN);

  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (query.next()) {
    Token token;
    DB_SET_TOKEN(token, query);
    ret.append(token);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

Alias
Database::selectAliasWithId(qint64 id) const
{
  DOUT("enter: id =" << id);
  //Q_ASSERT(id)

  Q_ASSERT(isValid());
  Alias ret;

  QSqlQuery query(db_);
  query.prepare(DB_SELECT_ALIAS "WHERE alias_id = ?");
  query.addBindValue(id);

  bool ok = query.exec();
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  DB_SET_ALIAS(ret, query);

  DOUT("exit: text =" << ret.text());
  return ret;
}

AliasList
Database::selectAliasesWithTokenId(qint64 tid) const
{
  DOUT("enter: tid =" << tid);
  //Q_ASSERT(tid)

  Q_ASSERT(isValid());
  AliasList ret;

  QSqlQuery query(db_);
  query.prepare(DB_SELECT_ALIAS "WHERE token_id = ?");
  query.addBindValue(tid);

  bool ok = query.exec();
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (query.next()) {
    Alias alias;
    DB_SET_ALIAS(alias, query);
    ret.append(alias);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

AliasList
Database::selectAliasesWithTokenDigest(const QString &digest, qint32 digestType)
{
  DOUT("enter: digestType =" << digestType);

  Q_ASSERT(isValid());
  AliasList ret;

  QSqlQuery query(db_);
  query.prepare(DB_SELECT_ALIAS "WHERE token_digest = ? AND token_digest_type = ?");
  query.addBindValue(digest);
  query.addBindValue(digestType);

  bool ok = query.exec();
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (query.next()) {
    Alias alias;
    DB_SET_ALIAS(alias, query);
    ret.append(alias);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

AliasList
Database::selectAliases() const
{
  DOUT("enter");

  Q_ASSERT(isValid());
  AliasList ret;

  QSqlQuery query(db_);
  bool ok = query.exec(DB_SELECT_ALIAS);

  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (query.next()) {
    Alias alias;
    DB_SET_ALIAS(alias, query);
    ret.append(alias);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}


Annotation
Database::selectAnnotationWithId(qint64 id) const
{
  DOUT("enter: id =" << id);
  //Q_ASSERT(id)

  Q_ASSERT(isValid());
  Annotation ret;

  QSqlQuery query(db_);
  query.prepare(DB_SELECT_ANNOTATION "WHERE annot_id =");
  query.addBindValue(id);

  bool ok = query.exec();
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  DB_SET_ANNOTATION(ret, query);

  DOUT("exit: text =" << ret.text());
  return ret;
}

AnnotationList
Database::selectAnnotationsWithTokenId(qint64 tid) const
{
  DOUT("enter: tid =" << tid);
  //Q_ASSERT(tid)

  Q_ASSERT(isValid());
  AnnotationList ret;

  QSqlQuery query(db_);
  query.prepare(DB_SELECT_ANNOTATION "WHERE token_id = ?");
  query.addBindValue(tid);

  bool ok = query.exec();
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (query.next()) {
    Annotation annot;
    DB_SET_ANNOTATION(annot, query);
    ret.append(annot);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

AnnotationList
Database::selectAnnotationsWithTokenDigest(const QString &digest, qint32 digestType) const
{
  DOUT("enter: digestType =" << digestType);

  Q_ASSERT(isValid());
  AnnotationList ret;

  QSqlQuery query(db_);
  query.prepare(DB_SELECT_ANNOTATION "WHERE token_digest = ? AND token_digest_type = ?");
  query.addBindValue(digest);
  query.addBindValue(digestType);

  bool ok = query.exec();
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (query.next()) {
    Annotation annot;
    DB_SET_ANNOTATION(annot, query);
    ret.append(annot);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

AnnotationList
Database::selectAnnotations() const
{
  DOUT("enter");

  Q_ASSERT(isValid());
  AnnotationList ret;

  QSqlQuery query(db_);
  bool ok = query.exec(DB_SELECT_ANNOTATION);

  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (query.next()) {
    Annotation annot;
    DB_SET_ANNOTATION(annot, query);
    ret.append(annot);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

// - Deletion -

void
Database::deleteUserWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM user WHERE user_id = ?");
  query.addBindValue(id);
  query.exec();
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteTokenWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM token WHERE token_id = ?");
  query.addBindValue(id);
  query.exec();
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteTokenWithDigest(const QString &digest, qint32 digestType)
{
  DOUT("enter: digestType =" << digestType);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM token WHERE token_digest = ? AND token_digest_type = ?");
  query.addBindValue(digest);
  query.addBindValue(digestType);
  query.exec();
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAliasWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM alias WHERE alias_id = ?");
  query.addBindValue(id);
  query.exec();
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAliasesWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM alias WHERE token_id = ?");
  query.addBindValue(tid);
  query.exec();
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAliasesWithTokenDigest(const QString &digest, qint32 digestType)
{
  DOUT("enter: digestType =" << digestType);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM alias WHERE token_digest = ? AND token_digest_type = ?");
  query.addBindValue(digest);
  query.addBindValue(digestType);
  query.exec();
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAnnotationWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM annot WHERE annot_id = ?");
  query.addBindValue(id);
  query.exec();
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAnnotationsWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM annot WHERE token_id = ?");
  query.addBindValue(tid);
  query.exec();
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAnnotationsWithTokenDigest(const QString &digest, qint32 digestType)
{
  DOUT("enter: digestType =" << digestType);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM annot WHERE token_digest = ? AND token_digest_type = ?");
  query.addBindValue(digest);
  query.addBindValue(digestType);
  query.exec();
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteUsers()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.exec("DELETE FROM user");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteTokens()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.exec("DELETE FROM token_alias");
  query.exec("DELETE FROM token");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAliases()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.exec("DELETE FROM alias");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAnnotations()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.exec("DELETE FROM annot");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

// EOF
