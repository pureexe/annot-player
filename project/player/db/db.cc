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

using namespace AnnotCloud;

namespace { // anonymous, helper

  inline QString uniqueString() { static int count = 0; return QString::number(++count); }

} // anonymous namespace

// - Threads -

namespace { namespace task_ { // anonymous

  class updateAnnotations : public QRunnable
  {
    Database *db_;
    AnnotationList l_;
    virtual void run() { db_->updateAnnotations(l_, false); } // \override, async = false
  public:
    updateAnnotations(const AnnotationList &l, Database *db) : db_(db), l_(l) { Q_ASSERT(db_); }
  };

} } // anonymous namespace task_

// - Constructions -

Database::Database(const QString &fileName, QObject *parent)
  : Base(parent), disposed_(false)
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
  DOUT("locking"); mutex_.lock(); DOUT("locked");

  if (db_.isOpen())
    db_.close();

  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  //db_.removeDatabase(db_.connectionName());
}

bool
Database::open(const QString &fileName)
{
  DOUT("fileName =" << fileName);

  // Locking is not needed currently
  //DOUT("locking"); mutex_.lock(); DOUT("locked");

  Q_ASSERT(!db_.isOpen());
  if (db_.isOpen())
    db_.close();
  db_.setDatabaseName(fileName);

  //DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  return db_.open();
}

void
Database::dispose()
{ disposed_ = true; }

void
Database::clear()
{
  DOUT("enter");

  DOUT("locking"); mutex_.lock(); DOUT("locked");

  if (db_.isOpen())
    db_.close();

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

  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");

  emit cleared();
  DOUT("exit");
}

// - Create -

bool
Database::createTables()
{
  DOUT("enter");
  Q_ASSERT(isValid());

  // Currently not needed
  //DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");

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

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");

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

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  if (!ok) {
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

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec(DB_SELECT_USER);
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");

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

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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
Database::selectTokenWithDigest(const QString &digest, qint32 part) const
{
  DOUT("enter: part =" << part << ", digest =" << digest);
  Q_ASSERT(isValid());
  //if (digest.isEmpty())
  //  return;
  Token ret;

  QSqlQuery query(db_);

  query.prepare(DB_SELECT_TOKEN "WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec(DB_SELECT_TOKEN);
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");

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

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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
Database::selectAliasesWithTokenDigest(const QString &digest, qint32 part)
{
  DOUT("enter: part =" << part);

  Q_ASSERT(isValid());
  AliasList ret;

  QSqlQuery query(db_);
  query.prepare(DB_SELECT_ALIAS "WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec(DB_SELECT_ALIAS);
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");

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
  query.prepare(DB_SELECT_ANNOTATION "WHERE annot_id = ?");
  query.addBindValue(id);

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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
Database::selectAnnotationsWithTokenDigest(const QString &digest, qint32 part) const
{
  DOUT("enter: part =" << part);

  Q_ASSERT(isValid());
  AnnotationList ret;

  QSqlQuery query(db_);
  query.prepare(DB_SELECT_ANNOTATION "WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
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
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec(DB_SELECT_ANNOTATION);
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");

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

qint64
Database::selectAnnotationUpdateTimeWithId(qint64 id) const
{
  DOUT("enter: id =" << id);
  //Q_ASSERT(id)

  Q_ASSERT(isValid());
  qint64 ret = -1;

  QSqlQuery query(db_);
  query.prepare(
    "SELECT "
      DB_UNIX_TIMESTAMP("annot_update_time") " "
    "FROM annot "
    "WHERE annot_id = ?"
  );
  query.addBindValue(id);

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  ret = query.value(0).toLongLong(&ok);
  if (!ok)
    ret = -1;

  DOUT("exit: text =" << ret);
  return ret;
}

// - Deletion -

bool
Database::deleteUserWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM user WHERE user_id = ?");
  query.addBindValue(id);

  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");

  DOUT("exit: affected rows =" << query.numRowsAffected());
  return query.numRowsAffected();
}

bool
Database::deleteTokenWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM token WHERE token_id = ?");
  query.addBindValue(id);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
  return query.numRowsAffected();
}

void
Database::deleteTokenWithDigest(const QString &digest, qint32 part)
{
  DOUT("enter: part =" << part);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM token WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

bool
Database::deleteAliasWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM alias WHERE alias_id = ?");
  query.addBindValue(id);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
  return query.numRowsAffected();
}

void
Database::deleteAliasesWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM alias WHERE token_id = ?");
  query.addBindValue(tid);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAliasesWithTokenDigest(const QString &digest, qint32 part)
{
  DOUT("enter: part =" << part);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM alias WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

bool
Database::deleteAnnotationWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM annot WHERE annot_id = ?");
  query.addBindValue(id);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
  return query.numRowsAffected();
}

void
Database::deleteAnnotationsWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM annot WHERE token_id = ?");
  query.addBindValue(tid);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAnnotationsWithTokenDigest(const QString &digest, qint32 part)
{
  DOUT("enter: part =" << part);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM annot WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteUsers()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec("DELETE FROM user");
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteTokens()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec("DELETE FROM token");
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAliases()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec("DELETE FROM alias");
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
Database::deleteAnnotations()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  DOUT("locking"); mutex_.lock(); DOUT("locked");
  query.exec("DELETE FROM annot");
  DOUT("unlocking"); mutex_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

// - Update -

void
Database::updateAnnotation(const Annotation &annot)
{
  DOUT("enter: aid =" << annot.id());
  if (!annot.hasId()) {
    DOUT("exit: ERROR: missing annot ID");
    return;
  }

  qint64 ts = selectAnnotationUpdateTimeWithId(annot.id());
  if (ts < 0)
    insertAnnotation(annot);
  else if (ts != annot.updateTime()) {
    deleteAnnotationWithId(annot.id());
    insertAnnotation(annot);
  }

  DOUT("exit");
  return;
}

void
Database::updateAnnotations(const AnnotationList &l, bool async)
{
  DOUT("enter: count =" << l.size() << ", async =" << async);
  Q_ASSERT(isValid());
  if (l.isEmpty() || disposed_) {
    DOUT("exit: empty list or disposed");
    return;
  }
  if (async) {
    QThreadPool::globalInstance()->start(new task_::updateAnnotations(l, this));
    DOUT("exit: returned from async branch");
    return;
  }

  foreach (Annotation a, l) {
    if (disposed_) {
      DOUT("exit: disposed");
      return;
    }
    updateAnnotation(a);
  }
  DOUT("exit");
  return;
}

// EOF
