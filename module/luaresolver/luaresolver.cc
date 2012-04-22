// luaresolver.cc
// 2/1/2012
// See: http://csl.sublevel3.org/lua/
#include "luaresolver.h"
#include "module/mrlanalysis/mrlanalysis.h"
#ifdef WITH_MODULE_LUACPP
#  include "module/luacpp/luacpp.h"
#else
#  error "luacpp is required"
#endif // WITH_MODULE_LUACPP
#ifdef WITH_MODULE_DOWNLOAD
#  include "module/download/downloader.h"
#else
#  error "download module is required"
#endif // WITH_MODULE_DOWNLOAD
#include "module/qtext/network.h"
#include "module/qtext/os.h"
#include <boost/function.hpp>
#include <exception>

#define _qs(_cstr)      QString::fromLocal8Bit(_cstr)

//#define DEBUG "luaresolver"
#include "module/debug/debug.h"

#ifdef __GNUC__
#  define NOINLINE      __attribute__((noinline))
#else
#  define NOINLINE
#endif // __GNUC__

// - Construction -

LuaResolver::LuaResolver(const QString &scriptPath, const QString &packagePath, QObject *parent)
  : Base(parent), scriptPath_(scriptPath), packagePath_(packagePath), cookieJar_(0)
{
#ifdef ANNOT_PROXY_DOMAIN
  cookieJar_ = new QtExt::NetworkCookieJarWithDomainAlias(".nicovideo.jp", ANNOT_PROXY_DOMAIN, this);
#else
#  warning "nico proxy domain is not defined"
#endif // ANNOT_PROXY_DOMAIN
}

// - Helpers -

namespace { // anonymous

  int NOINLINE clib_dlget_(lua_State *L)
  {
    LuaResolver *obj = LuaResolver::getObject(L);
    if (!obj)
      return 1;
    return obj->dlget(L);
  }

  int NOINLINE clib_dlpost_(lua_State *L)
  {
    LuaResolver *obj = LuaResolver::getObject(L);
    if (!obj)
      return 1;
    return obj->dlpost(L);
  }

} // anonymous namespace

/*
QString
LuaResolver::decodeText(const char *text, const char *encoding)
{
  if (text && encoding) {
    QTextCodec *tc = QTextCodec::codecForName(encoding);
    if (tc) {
      QTextDecoder *dc = tc->makeDecoder();
      if (dc)
        return dc->toUnicode(text);
    }
  }
  return QString::fromLocal8Bit(text);
}
*/

QString
LuaResolver::decodeTitle(const char *text, int siteId)
{
  switch (siteId) {
  case AcFun:
    //return decodeText(text, "GBK");
  case Bilibili:
  case Nicovideo:
  case UnknownSite:
  default:
    return QString::fromUtf8(text);
  }
}

void
LuaResolver::setObject(lua_State *L, Self *obj)
{
  DOUT("enter: obj =" << obj);
  lua_pushlightuserdata(L, 0);
  lua_pushlightuserdata(L, obj);
  lua_settable(L, LUA_REGISTRYINDEX);
  DOUT("exit");
}

LuaResolver*
LuaResolver::getObject(lua_State *L)
{
  DOUT("enter");
  lua_pushlightuserdata(L, 0);
  lua_gettable(L, LUA_REGISTRYINDEX);
  void *p = lua_isnil(L, -1) ? 0 : lua_touserdata(L, -1);
  lua_pop(L, 1);
  DOUT("exit: ret =" << p);
  return reinterpret_cast<Self*>(p);
}

// See: http://gamedevgeek.com/tutorials/calling-c-functions-from-lua/
// int dlget(string url, string path)
int
LuaResolver::dlget(lua_State *L)
{
  DOUT("enter");
  enum { PARAM_PATH = 1, PARAM_URL };
  Q_ASSERT(L);
  int param_count = lua_gettop(L);
  Q_ASSERT(param_count == 2);
  if (param_count != 2) {
    DOUT("exit: mismatched param_count =" << param_count);
    return 1;
  }
  QString path = _qs(lua_tostring(L, PARAM_PATH));
  QString url = _qs(lua_tostring(L, PARAM_URL));

  DOUT("url =" << url);
  Downloader *dl = new Downloader(path, this);
  if (cookieJar_)
    dl->networkAccessManager()->setCookieJar(cookieJar_);
  const QString noheader;
  dl->get(QUrl(url), noheader, false); // async = false
  bool ok = dl->state() == Downloader::OK;
  //if (ok && cookieJar_)
  //  hackCookieJar(cookieJar_);
  DOUT("exit: ok =" << ok);
  return ok ? 0 : 1;
}

int
LuaResolver::dlpost(lua_State *L)
{
  DOUT("enter");
  enum { PARAM_PATH = 1, PARAM_URL, PARAM_PARAM, PARAM_HEADER };
  Q_ASSERT(L);
  int param_count = lua_gettop(L);
  Q_ASSERT(param_count == 4);
  if (param_count != 4) {
    DOUT("exit: mismatched param_count =" << param_count);
    return 1;
  }
  QString path = _qs(lua_tostring(L, PARAM_PATH));
  QString url = _qs(lua_tostring(L, PARAM_URL));
  QString param = _qs(lua_tostring(L, PARAM_PARAM));
  QString header = _qs(lua_tostring(L, PARAM_HEADER));

  DOUT("url =" << url << ", path =" << path);
  Downloader *dl = new Downloader(path, this);
  if (cookieJar_)
    dl->networkAccessManager()->setCookieJar(cookieJar_);
  //QByteArray data = Downloader::encodeUrlParameters(param);
  QByteArray data = param.toLocal8Bit();
  dl->post(QUrl(url), data, header, false); // async = false
  bool ok = dl->state() == Downloader::OK;
  //if (ok && cookieJar_)
  //  hackCookieJar(cookieJar_);
  DOUT("exit: ok =" << ok);
  return ok ? 0 : 1;
}

// See: http://stackoverflow.com/questions/4125971/setting-the-global-lua-path-variable-from-c-c
// Append to package.path.

void
LuaResolver::printLastError(lua_State *L)
{
  DOUT("lua error:" << lua_tostring(L, -1));
  lua_pop(L, 1); // remove error message
}

void
LuaResolver::appendLuaPath(lua_State *L, const QString &path)
{
  DOUT("enter: path =" << path);
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "path"); // get field "path" from table at top of stack (-1)
  QString cur_path = _qs(lua_tostring(L, -1)); // grab path string from top of stack
  cur_path.append(";").append(path);
  lua_pop(L, 1); // get rid of the string on the stack we just pushed on line 5
  lua_pushstring(L, cur_path.toLocal8Bit()); // push the new one
  lua_setfield(L, -2, "path"); // set the field "path" in table at -2 with value at top of stack
  lua_pop(L, 1); // get rid of package table from top of stack
  DOUT("exit");
}

// - Resolvers -

bool
LuaResolver::resolve(const QString &href, int *siteid, QString *refurl, QString *title, QString *suburl,
                     QStringList *mrls, QList<qint64> *durations, QList<qint64> *sizes)
{
  if (href.isEmpty())
    return false;
  DOUT("href =" << href);

  lua_State *L = 0;

  try {
    L = lua_open();
    if (!L)
      return false;

    luaL_openlibs(L);

    // See: http://stackoverflow.com/questions/2710194/register-c-function-in-lua-table
    const luaL_Reg ft[] = {
      { "dlget", clib_dlget_ },
      { "dlpost", clib_dlpost_ },
      { 0, 0 }
    };
    luaL_register(L, "clib", ft);

    DOUT("scriptPath =" << scriptPath_);

    setObject(L, this);

    int err = luaL_loadfile(L, scriptPath_.toLocal8Bit());
    if (err) {
#ifdef DEBUG
      printLastError(L);
#endif // DEBUG
      lua_close(L);
      return false;
    }

    if (!packagePath_.isEmpty())
      appendLuaPath(L, packagePath_);

    err = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (err) {
#ifdef DEBUG
      printLastError(L);
#endif // DEBUG
      lua_close(L);
      return false;
    }

    // Set account
    if (hasNicovideoAccount() &&
        href.contains(MA_EIGEN_NICOVIDEO, Qt::CaseInsensitive)) {
      DOUT("nicovideo username =" << nicovideoUsername_);
      boost::function<int (std::string, std::string)>
          call = lua_function<int>(L, "set_nicovideo_account");
      err = call(nicovideoUsername_.toStdString(), nicovideoPassword_.toStdString());
      if (err) {
#ifdef DEBUG
        printLastError(L);
#endif // DEBUG
        lua_close(L);
        return false;
      }
    }

    if (hasBilibiliAccount() &&
        href.contains(MA_EIGEN_BILIBILI, Qt::CaseInsensitive)) {
      DOUT("bilibili username =" << bilibiliUsername_);
      boost::function<int (std::string, std::string)>
          call = lua_function<int>(L, "set_bilibili_account");
      err = call(bilibiliUsername_.toStdString(), bilibiliPassword_.toStdString());
      if (err) {
#ifdef DEBUG
        printLastError(L);
#endif // DEBUG
        lua_close(L);
        return false;
      }
    }

    // Invoke MRL resolver
    {
      const char *callee = mrls ? "resolve_media" : "resolve_subtitle";
      DOUT("callee =" << _qs(callee));
      // Must be consistent with resolve function in LuaResolver
      boost::function<int (std::string, std::string)>
          call = lua_function<int>(L, callee);
      QString dlfile = QtExt::mktemp();
      err = call(href.toStdString(), dlfile.toStdString());
      if (err) {
  #ifdef DEBUG
        printLastError(L);
  #endif // DEBUG
        lua_close(L);
        return false;
      }
    }

    int sid = 0;
    lua_getglobal(L, "g_siteid");
    if (!lua_isnil(L, -1))
      sid = lua_tointeger(L, -1);
    if (siteid)
      *siteid = sid;

    if (title) {
      lua_getglobal(L, "g_title");
      if (!lua_isnil(L, -1))
        *title = decodeTitle(lua_tostring(L, -1), sid);
    }

    if (refurl) {
      lua_getglobal(L, "g_refurl");
      if (!lua_isnil(L, -1))
        *refurl = _qs(lua_tostring(L, -1));
    }

    if (suburl) {
      lua_getglobal(L, "g_suburl");
      if (!lua_isnil(L, -1))
        *suburl = _qs(lua_tostring(L, -1));
    }

    if (mrls || durations || sizes) {
      int mrls_size = 0;
      lua_getglobal(L, "g_mrls_size");
      if (!lua_isnil(L, -1))
        mrls_size = lua_tointeger(L, -1);
      if (mrls_size > 0) {
        // MRLs
        if (mrls) {
          lua_getglobal(L, "g_mrls");
          if (!lua_isnil(L, -1))
            // See: http://www.wellho.net/mouth/1845_Passing-a-table-from-Lua-into-C.html
            for (int i = 0; i < mrls_size; i++) {
              QString k = QString::number(i);
              lua_pushstring(L, k.toAscii());
              lua_gettable(L, -2);
              mrls->append(_qs(lua_tostring(L,-1)));
              lua_pop(L,1);
            }
        }
        // Durations
        if (durations) {
          lua_getglobal(L, "g_durations");
          if (!lua_isnil(L, -1))
            for (int i = 0; i < mrls_size; i++) {
              QString k = QString::number(i);
              lua_pushstring(L, k.toAscii());
              lua_gettable(L, -2);
              durations->append(lua_tointeger(L,-1));
              lua_pop(L,1);
            }
        }
        // Sizes
        if (sizes) {
          lua_getglobal(L, "g_sizes");
          if (!lua_isnil(L, -1))
            for (int i = 0; i < mrls_size; i++) {
              QString k = QString::number(i);
              lua_pushstring(L, k.toAscii());
              lua_gettable(L, -2);
              sizes->append(lua_tointeger(L,-1));
              lua_pop(L,1);
            }
        }
      }
    }

#ifdef DEBUG
    // Debug
    {
      // 1 0 3 2 5 4 7 6
      int count = 0;
      lua_getglobal(L, "g_debug");
      if (!lua_isnil(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
          if (lua_isstring(L, -1)) {
            int index = count % 2 ? count + 1 : count - 1;
            DOUT("debug[" << index << "] =" << _qs(lua_tostring(L, -1)));
            count++;
          }
          lua_pop(L, 1);
        }
        lua_pop(L, 1);
      }
    }
#endif // DEBUG

    lua_close(L);

#ifdef DEBUG
    DOUT("siteid =" << sid);
    if (title)
      DOUT("title =" << *title);
    if (refurl)
      DOUT("refurl =" << *refurl);
    if (suburl)
      DOUT("suburl =" << *suburl);
    if (mrls)
      DOUT("mrls =" << *mrls);
    if (sizes)
      DOUT("sizes =" << *sizes);
    if (durations)
      DOUT("durations =" << *durations);
#endif // DEBUG
    return true;

  } catch (std::exception &e) {
#ifdef DEBUG
    std::cerr << DEBUG ":exception: " << e.what() << std::endl;
    if (L)
      printLastError(L);
#else
    Q_UNUSED(e);
#endif // DEBUG
    if (L)
      lua_close(L);
    return false;
  }
}

// EOF

/*
QString
LuaResolver::decodeUrl(const QString &url, const QString &href)
{
#ifndef USE_NICO_PROXY
  Q_UNUSED(href);
  return url;
#else
  if (url.isEmpty() ||
      !href.contains("nico.galstars.net/", Qt::CaseInsensitive))
    return url;

  // Example: http://smile-com00.nicovideo.jp/smile?m=17054368.25318low
  QRegExp rx("http://([\\w\\d\\-]+)\\.nicovideo\\.jp/smile\\?m=([\\w\\d\\.]+)", Qt::CaseInsensitive);
  if (!rx.exactMatch(url))
    return url;
  Q_ASSERT(rx.captureCount() == 2);
  QString t = rx.cap(1),
          m = rx.cap(2);
  // /: %2F, ?: %3F, see: http://www.blooberry.com/indexdot/html/topics/urlencoding.htm
  QString u = "%2Fsmile%3Fm=" + m;
  QString fmt = "http://nico.galstars.net/?t=%1&u=%2";
  return fmt.arg(t).arg(u);
#endif // USE_NICO_PROXY
}
*/
