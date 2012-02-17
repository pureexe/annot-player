#ifndef LUARESOLVER_H
#define LUARESOLVER_H

// luaresolver.h
// 2/2/2012
#include <QObject>
#include <QString>
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

struct lua_State;

class LuaResolver : public QObject
{
  Q_OBJECT
  typedef LuaResolver Self;
  typedef QObject Base;

  QNetworkAccessManager *nam_;

  QString scriptPath_;
  QString packagePath_;

  QString nicovideoUsername_, nicovideoPassword_;

  QString bilibiliUsername_, bilibiliPassword_;

public:
  //struct media_description
  //{
  //  std::string title;
  //  std::string refurl;
  //  std::string suburl;
  //  std::list<std::string> mrls;
  //};

  explicit LuaResolver(const QString &scriptPath,
                       const QString &packagePath = QString(),
                       QNetworkAccessManager *nam = 0,
                       QObject *parent = 0);

  QNetworkAccessManager *networkAccessManager() const
  { Q_ASSERT(nam_); return nam_; }

  // CHECKPOINT: siteid with nam
public:
  bool resolve(const QString &href,
               int *siteid = 0,
               QString *refurl = 0,
               QString *title = 0,
               QString *suburl = 0,
               QStringList *mrls = 0,
               QList<qint64> *durations = 0,
               QList<qint64> *sizes = 0);

  bool hasNicovideoAccount() const
  { return !nicovideoUsername_.isEmpty() && !nicovideoPassword_.isEmpty(); }

  bool hasBilibiliAccount() const
  { return !bilibiliUsername_.isEmpty() && !bilibiliPassword_.isEmpty(); }

public slots:
  void setNicovideoAccount(const QString &username, const QString &password)
  { nicovideoUsername_ = username; nicovideoPassword_ = password; }

  void clearNicovideoAccount()
  { nicovideoUsername_.clear(); nicovideoPassword_.clear(); }

  void setBilibiliAccount(const QString &username, const QString &password)
  { bilibiliUsername_ = username; bilibiliPassword_ = password; }

  void clearBilibiliAccount()
  { bilibiliUsername_.clear(); bilibiliPassword_.clear(); }

  // - Implementation -
public:
  static QString mktemp();

  static void setObject(lua_State *L, Self *obj);
  static Self *getObject(lua_State *L);

  int dlget(lua_State *L);
  int dlpost(lua_State *L);

protected:
  static void printLastError(lua_State *L);
  static void appendLuaPath(lua_State *L, const QString &path);
};

#endif // LUARESOLVER_H
