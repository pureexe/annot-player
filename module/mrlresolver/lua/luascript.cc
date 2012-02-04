// luascript.cc
// 2/1/2012
// See: http://csl.sublevel3.org/lua/
#include "luascript.h"
#include "module/luacpp/luacpp.h"
#include "module/download/download.h"
#include <boost/function.hpp>
#include <exception>
#include <iostream>
#include <cstdio>

#ifdef _MSC_VER
  #pragma warning (disable:4996)     // C4996: This function or variable may be unsafe. (tmpnam, getenv)
#endif // _MSC_VER

#define UNUSED(_var)    (void)(_var)

//#define DEBUG "luascript"
#include <module/debug/debug.h>

#define LUASCRIPT       "luascript.lua"

namespace  { // anonymous
  inline void perror_(lua_State *L)
  {
    std::cerr << "lua error: " << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1); // remove error message
  }

  // See: http://gamedevgeek.com/tutorials/calling-c-functions-from-lua/
  // int curl(string url, string path)
  int
  cc_curl_(lua_State *L)
  {
    Q_ASSERT(L);
    int param_count = lua_gettop(L);
    Q_ASSERT(param_count == 2);
    if (param_count != 2)
      return 1;
    QString url = QString::fromLocal8Bit(lua_tostring(L, 1));
    QString path = QString::fromLocal8Bit(lua_tostring(L, 2));
    bool zipped = url.contains("bilibili.tv/", Qt::CaseInsensitive);
    const bool async = false;
    bool ok = ::download(url, path, zipped, async);
    return ok ? 0 : 1;
  }
} // anonymous namespace

std::string
luascript::mktemp()
{
#ifdef _MSC_VER
  std::string tmp = ::getenv("tmp");
  if (tmp.empty())
    tmp = ::getenv("temp");
  return tmp + ::tmpnam(0);
#else
  return ::tmpnam(0);
#endif //_MSC_VER
}

bool
luascript::resolve_annot(const std::string &href, std::string &suburl)
{
  // TODO: not well-implemented
  media_description md;
  bool ret = resolve_media(href, md);
  if (ret)
    suburl = md.suburl;
  return ret;
}

bool
luascript::resolve_media(const std::string &href, media_description &md)
{
  if (href.empty())
    return false;

  lua_State *L = 0;

  try {
    L = lua_open();
    if (!L)
      return false;
    luaL_openlibs(L);

    // See: http://stackoverflow.com/questions/2710194/register-c-function-in-lua-table
    static const luaL_Reg ft[] = {
      { "curl", cc_curl_ },
      { 0, 0 }
    };
    luaL_register(L, "cc", ft);

    int err = luaL_loadfile(L, LUASCRIPT);
    if (err) {
#ifdef DEBUG
      perror_(L);
#endif // DEBUG
      lua_close(L);
      return false;
    }

    err = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (err) {
#ifdef DEBUG
      perror_(L);
#endif // DEBUG
      lua_close(L);
      return false;
    }

    // Must be consistent with resolve function in luascript
    boost::function<int (std::string, std::string)>
        resolve = lua_function<int>(L, "resolve_media");
    err = resolve(href, mktemp());
    if (err) {
#ifdef DEBUG
      perror_(L);
#endif // DEBUG
      lua_close(L);
      return false;
    }

    lua_getglobal(L, "g_mrls_size");
    int g_mrls_size = lua_tointeger(L, -1);
    if (g_mrls_size <= 0) {
      lua_close(L);
      return false;
    }

    lua_getglobal(L, "g_title");
    if (!lua_isnil(L, -1))
       md.title = lua_tostring(L, -1);
    else if (!md.title.empty())
      md.title.clear();

    lua_getglobal(L, "g_refurl");
    if (!lua_isnil(L, -1))
      md.refurl = lua_tostring(L, -1);
    else if (!md.refurl.empty())
      md.refurl.clear();

    lua_getglobal(L, "g_suburl");
    if (!lua_isnil(L, -1))
      md.suburl = lua_tostring(L, -1);
    else if (!md.suburl.empty())
      md.suburl.clear();

    if (!md.mrls.empty())
      md.mrls.clear();
    lua_getglobal(L, "g_mrls");
    if (!lua_isnil(L, -1)) {
      lua_pushnil(L);
      while (lua_next(L, -2)) {
        if (lua_isstring(L, -1))
          md.mrls.push_back(lua_tostring(L, -1));
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
    }

    lua_close(L);

#ifdef DEBUG
    std::cout << DEBUG ":title:" << md.title << "\n"
              << DEBUG ":refurl:" << md.refurl << "\n"
              << DEBUG ":suburl:" << md.suburl << "\n"
              << DEBUG ":mrls:" << std::endl;
    for (std::list<std::string>::const_iterator p = md.mrls.begin();
         p != md.mrls.end(); ++p)
       std::cout << *p << std::endl;
#endif // DEBUG
    return true;

  } catch (std::exception &e) {
#ifdef DEBUG
    std::cerr << DEBUG ":exception: " << e.what() << std::endl;
    if (L)
      perror_(L);
#else
    UNUSED(e);
#endif // DEBUG
    if (L)
      lua_close(L);
    return false;
  }
}

// EOF
