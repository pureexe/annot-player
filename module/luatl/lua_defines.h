#ifndef LUA_DEFINES_H
#define LUA_DEFINES_H

// lua_defines.h
// 6/10/2012
// See: http://www.lua.org/manual/5.2/manual.html

extern "C" {
#  include <lua.h>
} // extern "C"

// See: http://stackoverflow.com/questions/8552560/embedding-lua-in-c
#if LUA_VERSION_NUM < 502
# define luaL_newstate()  lua_open()
#endif // LUA_VERSION_NUM

// See: http://d.hatena.ne.jp/E_Mattsan/20120416/1334584047
// See: http://stackoverflow.com/questions/10704008/lua-api-evaluates-variable-name-instead-of-value
#if LUA_VERSION_NUM >= 502
# define luaL_register(_L, _name, _f) \
  { \
    lua_newtable((_L)); \
    lua_setglobal((_L), (_name)); \
    lua_getglobal((_L), (_name)); \
    luaL_setfuncs((_L), (_f), 0); \
    lua_settop((_L), 0); \
  }
#endif // LUA_VERSION_NUM

#endif // LUA_DEFINES_H
