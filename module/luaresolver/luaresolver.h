#ifndef LUARESOLVER_H
#define LUARESOLVER_H

// luaresolver.h
// 2/2/2012
#include <QObject>
#include <QString>
#include <QStringList>

QT_FORWARD_DECLARE_CLASS(QNetworkCookieJar)

struct lua_State;

class LuaResolver : public QObject
{
  Q_OBJECT
  typedef LuaResolver Self;
  typedef QObject Base;

  QString scriptPath_;
  QString packagePath_;

  QString nicovideoUsername_, nicovideoPassword_;

  QString bilibiliUsername_, bilibiliPassword_;

  QNetworkCookieJar *cookieJar_;

public:
  explicit LuaResolver(const QString &scriptPath,
                       const QString &packagePath = QString(),
                       QObject *parent = 0);

public:
  enum Site { UnknownSite = 0, AcFun = 1, Nicovideo = 2, Bilibili = 3 };

  bool resolve(const QString &href,
               int *siteid = 0,
               QString *refurl = 0,
               QString *title = 0,
               QString *suburl = 0,
               QStringList *mrls = 0,
               QList<qint64> *durations = 0,
               QList<qint64> *sizes = 0);

  QNetworkCookieJar *cookieJar() const { return cookieJar_; }
  void setCookieJar(QNetworkCookieJar *cookieJar) { cookieJar_ = cookieJar; }

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
  static void setObject(lua_State *L, Self *obj);
  static Self *getObject(lua_State *L);

  int dlget(lua_State *L);
  int dlpost(lua_State *L);

protected:
  static void printLastError(lua_State *L);
  static void appendLuaPath(lua_State *L, const QString &path);
  static QString decodeText(const char *text, const char *encoding);
  static QString decodeTitle(const char *text, int siteId);
  //static QString decodeUrl(const QString &url, const QString &href);
  //static void hackCookieJar(QNetworkCookieJar *jar);
};

#endif // LUARESOLVER_H
