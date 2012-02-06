// luaresolver.cc
// 2/1/2012
// See: http://csl.sublevel3.org/lua/
#include "luaresolver.h"
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

//#include <QtCore>
//#define DEBUG "luaresolver"
#include <module/debug/debug.h>

// - Helpers -

namespace  { // anonymous
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
luaresolver::mktemp()
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

// See: http://stackoverflow.com/questions/4125971/setting-the-global-lua-path-variable-from-c-c
// Append to package.path.

void
luaresolver::print_last_error(lua_State *L)
{
  std::cerr << "lua error: " << lua_tostring(L, -1) << std::endl;
  lua_pop(L, 1); // remove error message
}

void
luaresolver::append_lua_path(lua_State *L, const char *path)
{
  DOUT("enter: path =" << QString::fromStdString(path));
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "path"); // get field "path" from table at top of stack (-1)
  std::string cur_path = lua_tostring(L, -1); // grab path string from top of stack
  cur_path.append(";"); // do your path magic here
  cur_path.append(path);
  lua_pop(L, 1); // get rid of the string on the stack we just pushed on line 5
  lua_pushstring(L, cur_path.c_str()); // push the new one
  lua_setfield(L, -2, "path"); // set the field "path" in table at -2 with value at top of stack
  lua_pop(L, 1); // get rid of package table from top of stack
  DOUT("exit");
}

// - Resolvers -

bool
luaresolver::resolve_annot(const std::string &href, std::string &suburl) const
{
  // TODO: not well-implemented
  DOUT("enter: href =" << QString::fromStdString(href));
  media_description md;
  bool ret = resolve_media(href, md);
  if (ret)
    suburl = md.suburl;
  DOUT("exit: ret =" << ret << ", suburl =" << QString::fromStdString(md.suburl));
  return ret;
}

bool
luaresolver::resolve_media(const std::string &href, media_description &md) const
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

    DOUT("script_path =" << QString::fromStdString(script_path_));

    int err = luaL_loadfile(L, script_path_.c_str());
    if (err) {
#ifdef DEBUG
      print_last_error(L);
#endif // DEBUG
      lua_close(L);
      return false;
    }

    append_lua_path(L);

    err = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (err) {
#ifdef DEBUG
      print_last_error(L);
#endif // DEBUG
      lua_close(L);
      return false;
    }

    // Must be consistent with resolve function in luaresolver
    boost::function<int (std::string, std::string)>
        resolve = lua_function<int>(L, "resolve_media");
    err = resolve(href, mktemp());
    if (err) {
#ifdef DEBUG
      print_last_error(L);
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
      print_last_error(L);
#else
    UNUSED(e);
#endif // DEBUG
    if (L)
      lua_close(L);
    return false;
  }
}

// EOF
