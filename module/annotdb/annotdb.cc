// annotdb.cc
// 9/12/2011
//
// Note 9/13/2011: SQLite doesn't allow insert multple rows at the same time.
// The way to bypass this is ugly and might not be more efficient.
//
// SQLite functions:
// - date and time: http://www.sqlite.org/lang_datefunc.html
// - hex: http://www.sqlite.org/lang_corefunc.html
//   unhex: not available
//
// *** Tuning performance on Windows ***
// See: http://stackoverflow.com/questions/1711631/how-do-i-improve-the-performance-of-sqlite

#include "module/annotdb/annotdb.h"
#include "module/annotdb/annotdbdefs.h"
#include <QtCore>
#include <QtSql>

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wparentheses" // suggest parentheses around && within ||
#endif // __GNUC__

//#define DEBUG "annotdb"
#include "module/debug/debug.h"

using namespace AnnotCloud;

//#ifdef Q_OS_WIN
//#  define ANNOTDB_TUNE
//#endif // Q_OS_WIN

// - Threads -

namespace { namespace task_ { // anonymous

  class updateAliases : public QRunnable
  {
    AnnotationDatabase *db_;
    AliasList l_;
    int limit_;
    virtual void run() { db_->updateAliases(l_, false, limit_); } // \override, async = false
  public:
    updateAliases(const AliasList &l, int limit, AnnotationDatabase *db)
      : db_(db), l_(l), limit_(limit) { Q_ASSERT(db_); }
  };

  class updateAnnotations : public QRunnable
  {
    AnnotationDatabase *db_;
    AnnotationList l_;
    int limit_;
    virtual void run() { db_->updateAnnotations(l_, false, limit_); } // \override, async = false
  public:
    updateAnnotations(const AnnotationList &l, int limit, AnnotationDatabase *db)
      : db_(db), l_(l), limit_(limit) { Q_ASSERT(db_); }
  };

} } // anonymous namespace task_

// - Constructions -

AnnotationDatabase::~AnnotationDatabase()
{
  DOUT("enter");
  QMutexLocker lock(&m_);

  if (db_.isOpen())
    db_.close();
  DOUT("exit");
}

QString
AnnotationDatabase::newId()
{
  static int id = 0;
  return QString::number(++id);
}

bool
AnnotationDatabase::open(const QString &fileName)
{
  DOUT("enter: fileName =" << fileName);
  QFileInfo fi(fileName);
  bool empty = !fi.exists() || fi.size() == 0;

  db_ = QSqlDatabase::addDatabase("QSQLITE", newId());
  bool ok = db_.isValid();
  if (ok) {
    Q_ASSERT(db_.driver() && db_.driver()->hasFeature(QSqlDriver::LastInsertId));
    db_.setDatabaseName(fileName);
    ok = db_.open();
    if (ok)
      tune();
  }
  if (ok && empty) {
    ok = createTables();
    if (!ok) {
      DOUT("failed to create database tables");
      db_.close();
      DOUT("try to remove corrupted db:" << fileName);
      QFile::remove(fileName);
    }
  }

  ok = isValid();
  DOUT("exit: ret =" << ok);
  return ok;
}

void
AnnotationDatabase::clear()
{
  DOUT("enter");
  QMutexLocker lock(&m_);

  if (db_.isOpen())
    db_.close();

  db_.removeDatabase(db_.connectionName());

  QString fileName = db_.databaseName();
  if (QFileInfo(fileName).exists()) {
    DOUT("try to remove old db file:" << fileName);
    QFile::remove(fileName);
  }
  db_ = QSqlDatabase::addDatabase("QSQLITE", newId());
  bool ok = db_.isValid();
  if (ok) {
    db_.setDatabaseName(fileName);
    ok = db_.open();
    if (ok)
      tune();
  }

  if (ok) {
    ok = createTables();
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

// See: http://www.qtcentre.org/archive/index.php/t-39168.html
void
AnnotationDatabase::tune()
{
#ifdef ANNOTDB_TUNE
  DOUT("enter");
  Q_ASSERT(isValid());

  bool ok = QSqlQuery(db_).exec("PRAGMA journal_mode = OFF");
  Q_UNUSED(ok);

  DOUT("exit: ok =" << ok);
#endif // ANNOTDB_TUNE
}

bool
AnnotationDatabase::createTables()
{
  DOUT("enter");
  Q_ASSERT(isValid());

  // Currently not needed
  //DOUT("unlocking"); m_.unlock(); DOUT("unlocked");

  QSqlQuery query(db_);

  DOUT("creating table 'user' ...");
  bool ok = query.exec(ANNOTDB_CREATE_TABLE_USER);
  if (!ok) {
    DOUT("exit: failed to create 'user', error:" << query.lastError());
    return false;
  } else
    DOUT("succeeded");

  DOUT("creating table 'token' ...");
  ok = query.exec(ANNOTDB_CREATE_TABLE_TOKEN);
  if (!ok) {
    DOUT("exit: failed to create 'token', error:" << query.lastError());
    return false;
  } else
    DOUT("succeeded");

  DOUT("creating table 'alias' ...");
  ok = query.exec(ANNOTDB_CREATE_TABLE_ALIAS);
  if (!ok) {
    DOUT("exit: failed to create 'token_alias', error:" << query.lastError());
    return false;
  } else
    DOUT("succeeded");

  DOUT("creating table 'annot' ...");
  ok = query.exec(ANNOTDB_CREATE_TABLE_ANNOT);
  if (!ok) {
    DOUT("exit: failed to create 'annot', error:" << query.lastError());
    return false;
  } else
    DOUT("exit: succeeded");

  return true;
}

// - Insertion -

qint64
AnnotationDatabase::insertUser(const User& user)
{
  DOUT("enter: username =" << user.name() << ", id =" << user.id());
  Q_ASSERT(isValid());
  QSqlQuery query(db_);

  ANNOTDB_INSERT_USER(user, query);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");

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
AnnotationDatabase::insertUsers(const UserList &l)
{
  DOUT("enter: count =" << l.size());
  Q_ASSERT(isValid());
  bool ok = true;
  foreach (const User &u, l)
    if (disposed_ || !insertUser(u)) {
      DOUT("failed at id =" << u.id() << ", ignored");
      ok = false;
    }
  DOUT("exit: ret =" << ok);
  return ok;
}

qint64
AnnotationDatabase::insertToken(const Token &token)
{
  DOUT("enter: id =" << token.id());
  Q_ASSERT(isValid());
  QSqlQuery query(db_);

  ANNOTDB_INSERT_TOKEN(token, query);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
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
AnnotationDatabase::insertTokens(const TokenList &l)
{
  DOUT("enter: count =" << l.size());
  Q_ASSERT(isValid());
  bool ok = true;
  foreach (const Token &t, l)
    if (disposed_ || !insertToken(t)) {
      DOUT("failed at id =" << t.id() << ", ignored");
      ok = false;
    }
  DOUT("exit: ret =" << ok);
  return ok;
}

qint64
AnnotationDatabase::insertAlias(const Alias &alias)
{
  DOUT("enter: id =" << alias.id()
       << ", tid =" << alias.tokenId()
       << ", uid =" << alias.userId()
       << ", text =" << alias.text());
  Q_ASSERT(isValid());
  QSqlQuery query(db_);

  ANNOTDB_INSERT_ALIAS(alias, query);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
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
AnnotationDatabase::isAliasExists(const Alias &alias) const
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

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("error:" << query.lastError());
    return false;
  }

  bool ret = query.size();
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
AnnotationDatabase::insertAliases(const AliasList &l)
{
  DOUT("enter: count =" << l.size());
  Q_ASSERT(isValid());
  bool ok = true;
  foreach (const Alias &a, l)
    if (disposed_ || !isAliasExists(a) && !insertAlias(a)) {
       DOUT("failed at id =" << a.id() << ", ignored");
       ok = false;
    }
  DOUT("exit: ret =" << ok);
  return ok;
}

qint64
AnnotationDatabase::insertAnnotation(const Annotation &annot, bool lock)
{
  DOUT("enter: id =" << annot.id()
       << ", tid =" << annot.tokenId()
       << ", uid =" << annot.userId());
  Q_ASSERT(isValid());
  QSqlQuery query(db_);

  ANNOTDB_INSERT_ANNOT(annot, query);

  if (lock) { DOUT("locking"); m_.lock(); DOUT("locked"); }
  bool ok = query.exec();
  if (lock) { DOUT("unlocking"); m_.unlock(); DOUT("unlocked"); }
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
AnnotationDatabase::insertAnnotations(const AnnotationList &l)
{
  DOUT("enter: count =" << l.size());
  Q_ASSERT(isValid());
  if (l.isEmpty())
    return true;

  qDebug() << "AnnotationDatabase::insertAnnotations:enter: count =" << l.size();

  bool ok = true;

#ifdef Q_WS_WIN
  enum { TransactionLimit = 10 };
#else
  enum { TransactionLimit = 100 };
#endif // Q_WS_WIN

  if (TransactionLimit && l.size() > TransactionLimit) {
    QMutexLocker lock(&m_);
#ifdef ANNOTDB_TUNE
    bool tuned = QSqlQuery(db_).exec("PRAGMA synchronous = OFF");
#endif // ANNOTDB_TUNE
    bool t = db_.transaction();
    foreach (const Annotation &a, l)
      if (disposed_ || !insertAnnotation(a, false)) { // lock = false
        DOUT("failed at id =" << a.id() << ", ignored");
        ok = false;
      }
    if (t)
      ok = db_.commit() && ok;
    DOUT("transaction =" << t);
#ifdef ANNOTDB_TUNE
    if (tuned)
      QSqlQuery(db_).exec("PRAGMA synchronous = ON");
    DOUT("tuned =" << tuned);
#endif // ANNOTDB_TUNE
  } else
    foreach (const Annotation &a, l)
      if (disposed_ || !insertAnnotation(a)) {
        DOUT("failed at id =" << a.id() << ", ignored");
        ok = false;
      }

  qDebug() << "AnnotationDatabase::insertAnnotations:exit: ret =" << ok;
  DOUT("exit: ret =" << ok);
  return ok;
}

// - Selection -

User
AnnotationDatabase::selectUserWithNameAndPassword(const QString &name, const QString &password) const
{
  DOUT("enter: name =" << name << ", password =" << password);
  Q_ASSERT(isValid());
  User ret;

  QSqlQuery query(db_);
  query.prepare(ANNOTDB_SELECT_USER "WHERE user_name = ? AND user_password = ?");
  query.addBindValue(name);     // 1:user_name
  query.addBindValue(password); // 2:user_password

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }
  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  ANNOTDB_SET_USER(ret, query);

  DOUT("exit: userid =" << ret.id());
  return ret;
}

User
AnnotationDatabase::selectUserWithId(qint64 id) const
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  //Q_ASSERT(id);
  User ret;

  QSqlQuery query(db_);
  query.prepare(ANNOTDB_SELECT_USER "WHERE user_id = ?");
  query.addBindValue(id);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }
  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  ANNOTDB_SET_USER(ret, query);

  DOUT("exit: username =" << ret.name());
  return ret;
}

UserList
AnnotationDatabase::selectUsers() const
{
  DOUT("enter");
  Q_ASSERT(isValid());
  UserList ret;

  QSqlQuery query(db_);
  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec(ANNOTDB_SELECT_USER);
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");

  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (!disposed_ && query.next()) {
    User user;
    ANNOTDB_SET_USER(user, query);
    ret.append(user);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}


Token
AnnotationDatabase::selectTokenWithId(qint64 id) const
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  //Q_ASSERT(id);
  Token ret;

  QSqlQuery query(db_);

  query.prepare(ANNOTDB_SELECT_TOKEN "WHERE token_id = ?");
  query.addBindValue(id);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }
  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  ANNOTDB_SET_TOKEN(ret, query);

  DOUT("exit: digest =" << ret.digest());
  return ret;
}

Token
AnnotationDatabase::selectTokenWithDigest(const QString &digest, qint32 part) const
{
  DOUT("enter: part =" << part << ", digest =" << digest);
  Q_ASSERT(isValid());
  //if (digest.isEmpty())
  //  return;
  Token ret;

  QSqlQuery query(db_);

  query.prepare(ANNOTDB_SELECT_TOKEN "WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }
  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  ANNOTDB_SET_TOKEN(ret, query);

  DOUT("exit: id =" << ret.id());
  return ret;
}

TokenList
AnnotationDatabase::selectTokens() const
{
  DOUT("enter");
  Q_ASSERT(isValid());
  TokenList ret;

  QSqlQuery query(db_);
  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec(ANNOTDB_SELECT_TOKEN);
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");

  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (!disposed_ && query.next()) {
    Token token;
    ANNOTDB_SET_TOKEN(token, query);
    ret.append(token);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

Alias
AnnotationDatabase::selectAliasWithId(qint64 id) const
{
  DOUT("enter: id =" << id);
  //Q_ASSERT(id)

  Q_ASSERT(isValid());
  Alias ret;

  QSqlQuery query(db_);
  query.prepare(ANNOTDB_SELECT_ALIAS "WHERE alias_id = ?");
  query.addBindValue(id);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  ANNOTDB_SET_ALIAS(ret, query);

  DOUT("exit: text =" << ret.text());
  return ret;
}

AliasList
AnnotationDatabase::selectAliasesWithTokenId(qint64 tid) const
{
  DOUT("enter: tid =" << tid);
  //Q_ASSERT(tid)

  Q_ASSERT(isValid());
  AliasList ret;

  QSqlQuery query(db_);
  query.prepare(ANNOTDB_SELECT_ALIAS "WHERE token_id = ?");
  query.addBindValue(tid);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (!disposed_ && query.next()) {
    Alias alias;
    ANNOTDB_SET_ALIAS(alias, query);
    ret.append(alias);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

AliasList
AnnotationDatabase::selectAliasesWithTokenDigest(const QString &digest, qint32 part)
{
  DOUT("enter: part =" << part);

  Q_ASSERT(isValid());
  AliasList ret;

  QSqlQuery query(db_);
  query.prepare(ANNOTDB_SELECT_ALIAS "WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (!disposed_ && query.next()) {
    Alias alias;
    ANNOTDB_SET_ALIAS(alias, query);
    ret.append(alias);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

AliasList
AnnotationDatabase::selectAliases() const
{
  DOUT("enter");

  Q_ASSERT(isValid());
  AliasList ret;

  QSqlQuery query(db_);
  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec(ANNOTDB_SELECT_ALIAS);
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");

  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  while (!disposed_ && query.next()) {
    Alias alias;
    ANNOTDB_SET_ALIAS(alias, query);
    ret.append(alias);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

Annotation
AnnotationDatabase::selectAnnotationWithId(qint64 id) const
{
  DOUT("enter: id =" << id);
  //Q_ASSERT(id)

  Q_ASSERT(isValid());
  Annotation ret;

  QSqlQuery query(db_);
  query.prepare(ANNOTDB_SELECT_ANNOTATION "WHERE annot_id = ?");
  query.addBindValue(id);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  if (!query.next()) {
    DOUT("exit: query returns empty");
    return ret;
  }

  ANNOTDB_SET_ANNOTATION(ret, query);

  DOUT("exit: text =" << ret.text());
  return ret;
}

AnnotationList
AnnotationDatabase::selectAnnotationsWithTokenId(qint64 tid) const
{
  DOUT("enter: tid =" << tid);
  //Q_ASSERT(tid)

  Q_ASSERT(isValid());
  AnnotationList ret;

  QSqlQuery query(db_);
  query.prepare(ANNOTDB_SELECT_ANNOTATION "WHERE token_id = ?");
  query.addBindValue(tid);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  Annotation annot; // move out for better performance
  while (!disposed_ && query.next()) {
    ANNOTDB_SET_ANNOTATION(annot, query);
    ret.append(annot);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

AnnotationList
AnnotationDatabase::selectAnnotationsWithTokenDigest(const QString &digest, qint32 part) const
{
  DOUT("enter: part =" << part);

  Q_ASSERT(isValid());
  AnnotationList ret;

  QSqlQuery query(db_);
  query.prepare(ANNOTDB_SELECT_ANNOTATION "WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  Annotation annot; // move out for better performance
  while (!disposed_ && query.next()) {
    ANNOTDB_SET_ANNOTATION(annot, query);
    ret.append(annot);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

AnnotationList
AnnotationDatabase::selectAnnotations() const
{
  DOUT("enter");

  Q_ASSERT(isValid());
  AnnotationList ret;

  QSqlQuery query(db_);
  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec(ANNOTDB_SELECT_ANNOTATION);
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");

  if (!ok) {
    DOUT("exit: error:" << query.lastError());
    return ret;
  }

  Annotation annot; // move out for better performance
  while (!disposed_ && query.next()) {
    ANNOTDB_SET_ANNOTATION(annot, query);
    ret.append(annot);
  }

  DOUT("exit: count =" << ret.size());
  return ret;
}

qint64
AnnotationDatabase::selectAliasUpdateTimeWithId(qint64 id) const
{
  DOUT("enter: id =" << id);
  //Q_ASSERT(id)

  Q_ASSERT(isValid());
  qint64 ret = -1;

  QSqlQuery query(db_);
  query.prepare(
    "SELECT "
      ANNOTDB_UNIX_TIMESTAMP("alias_update_time") " "
    "FROM alias "
    "WHERE alias_id = ?"
  );
  query.addBindValue(id);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
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

qint64
AnnotationDatabase::selectAnnotationUpdateTimeWithId(qint64 id) const
{
  DOUT("enter: id =" << id);
  //Q_ASSERT(id)

  Q_ASSERT(isValid());
  qint64 ret = -1;

  QSqlQuery query(db_);
  query.prepare(
    "SELECT "
      ANNOTDB_UNIX_TIMESTAMP("annot_update_time") " "
    "FROM annot "
    "WHERE annot_id = ?"
  );
  query.addBindValue(id);

  DOUT("locking"); m_.lock(); DOUT("locked");
  bool ok = query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
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
AnnotationDatabase::deleteUserWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM user WHERE user_id = ?");
  query.addBindValue(id);

  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");

  DOUT("exit: affected rows =" << query.numRowsAffected());
  return query.numRowsAffected();
}

bool
AnnotationDatabase::deleteTokenWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM token WHERE token_id = ?");
  query.addBindValue(id);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
  return query.numRowsAffected();
}

void
AnnotationDatabase::deleteTokenWithDigest(const QString &digest, qint32 part)
{
  DOUT("enter: part =" << part);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM token WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

bool
AnnotationDatabase::deleteAliasWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM alias WHERE alias_id = ?");
  query.addBindValue(id);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
  return query.numRowsAffected();
}

void
AnnotationDatabase::deleteAliasesWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM alias WHERE token_id = ?");
  query.addBindValue(tid);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
AnnotationDatabase::deleteAliasesWithTokenDigest(const QString &digest, qint32 part)
{
  DOUT("enter: part =" << part);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM alias WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

bool
AnnotationDatabase::deleteAnnotationWithId(qint64 id)
{
  DOUT("enter: id =" << id);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM annot WHERE annot_id = ?");
  query.addBindValue(id);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
  return query.numRowsAffected();
}

void
AnnotationDatabase::deleteAnnotationsWithTokenId(qint64 tid)
{
  DOUT("enter: tid =" << tid);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM annot WHERE token_id = ?");
  query.addBindValue(tid);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
AnnotationDatabase::deleteAnnotationsWithTokenDigest(const QString &digest, qint32 part)
{
  DOUT("enter: part =" << part);
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  query.prepare("DELETE FROM annot WHERE token_digest = ? AND token_part = ?");
  query.addBindValue(digest);
  query.addBindValue(part);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec();
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
AnnotationDatabase::deleteUsers()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec("DELETE FROM user");
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
AnnotationDatabase::deleteTokens()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec("DELETE FROM token");
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
AnnotationDatabase::deleteAliases()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec("DELETE FROM alias");
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

void
AnnotationDatabase::deleteAnnotations()
{
  DOUT("enter");
  Q_ASSERT(isValid());
  QSqlQuery query(db_);
  DOUT("locking"); m_.lock(); DOUT("locked");
  query.exec("DELETE FROM annot");
  DOUT("unlocking"); m_.unlock(); DOUT("unlocked");
  DOUT("exit: affected rows =" << query.numRowsAffected());
}

// - Update -

void
AnnotationDatabase::updateAnnotation(const Annotation &annot)
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
AnnotationDatabase::updateAnnotations(const AnnotationList &l, bool async, int limit)
{
  DOUT("enter: count =" << l.size() << ", limit =" << limit << ", async =" << async);
  Q_ASSERT(isValid());
  if (l.isEmpty() || disposed_) {
    DOUT("exit: empty list or disposed");
    return;
  }
  if (async) {
    QThreadPool::globalInstance()->start(new task_::updateAnnotations(l, limit, this));
    DOUT("exit: returned from async branch");
    return;
  }

  int count = 0;
  foreach (const Annotation &a, l) {
    if (disposed_ ||
        limit && count++ >= limit) {
      DOUT("exit: disposed or limit reached");
      return;
    }
    updateAnnotation(a);
  }
  DOUT("exit");
  return;
}

void
AnnotationDatabase::updateAlias(const Alias &alias)
{
  DOUT("enter: aid =" << alias.id());
  if (!alias.hasId()) {
    DOUT("exit: ERROR: missing alias ID");
    return;
  }

  qint64 ts = selectAliasUpdateTimeWithId(alias.id());
  if (ts < 0)
    insertAlias(alias);
  else if (ts != alias.updateTime()) {
    deleteAliasWithId(alias.id());
    insertAlias(alias);
  }

  DOUT("exit");
  return;
}

void
AnnotationDatabase::updateAliases(const AliasList &l, bool async, int limit)
{
  DOUT("enter: count =" << l.size() << ", limit =" << limit << ", async =" << async);
  Q_ASSERT(isValid());
  if (l.isEmpty() || disposed_) {
    DOUT("exit: empty list or disposed");
    return;
  }
  if (async) {
    QThreadPool::globalInstance()->start(new task_::updateAliases(l, limit, this));
    DOUT("exit: returned from async branch");
    return;
  }

  int count = 0;
  foreach (const Alias &a, l) {
    if (disposed_ ||
        limit && count++ >= limit) {
      DOUT("exit: disposed or limit reached");
      return;
    }
    updateAlias(a);
  }
  DOUT("exit");
  return;
}
// EOF
