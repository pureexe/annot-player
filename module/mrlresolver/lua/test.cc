// 2/1/2012
// See: http://csl.sublevel3.org/lua/
extern "C" {
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
} // extern "C"
#include <boost/function.hpp>
#include <iostream>
#include <string>
#include <list>
#include <exception>
#include <stdexcept>

#define DEBUG "imagecreator::load"

namespace detail
{
    // we overload push_value instead of specializing
    // because this way we can also push values that
    // are implicitly convertible to one of the types

    void push_value(lua_State *vm, lua_Integer n)
    {
        lua_pushinteger(vm, n);
    }

    void push_value(lua_State *vm, lua_Number n)
    {
        lua_pushnumber(vm, n);
    }

    void push_value(lua_State *vm, bool b)
    {
        lua_pushboolean(vm, b);
    }

    void push_value(lua_State *vm, const std::string& s)
    {
        lua_pushstring(vm, s.c_str());
    }

    // other overloads, for stuff like userdata or C functions

    // for extracting return values, we specialize a simple struct
    // as overloading on return type does not work, and we only need
    // to support a specific set of return types, as the return type
    // of a function is always specified explicitly

    template <typename T>
    struct value_extractor
    {
    };

    template <>
    struct value_extractor<lua_Integer>
    {
        static lua_Integer get(lua_State *vm)
        {
            lua_Integer val = lua_tointeger(vm, -1);
            lua_pop(vm, 1);
            return val;
        }
    };

    template <>
    struct value_extractor<lua_Number>
    {
        static lua_Number get(lua_State *vm)
        {
            lua_Number val = lua_tonumber(vm, -1);
            lua_pop(vm, 1);
            return val;
        }
    };

    template <>
    struct value_extractor<bool>
    {
        static bool get(lua_State *vm)
        {
            bool val = lua_toboolean(vm, -1);
            lua_pop(vm, 1);
            return val;
        }
    };

    template <>
    struct value_extractor<std::string>
    {
        static std::string get(lua_State *vm)
        {
            std::string val = lua_tostring(vm, -1);
            lua_pop(vm, 1);
            return val;
        }
    };

    template <>
    struct value_extractor<int>
    {
        static int get(lua_State *vm)
        {
            int val = lua_tonumber(vm, -1);
            lua_pop(vm, 1);
            return val;
        }
    };
    // other specializations, for stuff like userdata or C functions
} // detail

// the base function wrapper class
class lua_function_base
{
public:
    lua_function_base(lua_State *vm, const std::string& func)
        : m_vm(vm)
    {
        // get the function
        lua_getfield(m_vm, LUA_GLOBALSINDEX, func.c_str());
        // ensure it's a function
        if (!lua_isfunction(m_vm, -1)) {
            // throw an exception; you'd use your own exception class here
            // of course, but for sake of simplicity i use runtime_error
            lua_pop(m_vm, 1);
            throw std::runtime_error("not a valid function");
        }
        // store it in registry for later use
        m_func = luaL_ref(m_vm, LUA_REGISTRYINDEX);
    }

    lua_function_base(const lua_function_base& func)
        : m_vm(func.m_vm)
    {
        // copy the registry reference
        lua_rawgeti(m_vm, LUA_REGISTRYINDEX, func.m_func);
        m_func = luaL_ref(m_vm, LUA_REGISTRYINDEX);
    }

    ~lua_function_base()
    {
        // delete the reference from registry
        luaL_unref(m_vm, LUA_REGISTRYINDEX, m_func);
    }

    lua_function_base& operator=(const lua_function_base& func)
    {
        if (this != &func) {
            m_vm = func.m_vm;
            lua_rawgeti(m_vm, LUA_REGISTRYINDEX, func.m_func);
            m_func = luaL_ref(m_vm, LUA_REGISTRYINDEX);
        }
        return *this;
    }
protected:
    // the virtual machine and the registry reference to the function
    lua_State *m_vm;
    int m_func;

    // call the function, throws an exception on error
    void call(int args, int results = 0)
    {
        // call it with no return values
        int status = lua_pcall(m_vm, args, results, 0);
        if (status != 0) {
            // call failed; throw an exception
            std::string error = lua_tostring(m_vm, -1);
            lua_pop(m_vm, 1);
            // in reality you'd want to use your own exception class here
            throw std::runtime_error(error.c_str());
        }
    }
};

// the function wrapper class
template <typename Ret>
class lua_function : public lua_function_base
{
public:
    lua_function(lua_State *vm, const std::string& func)
        : lua_function_base(vm, func)
    {
    }

    Ret operator()()
    {
        // push the function from the registry
        lua_rawgeti(m_vm, LUA_REGISTRYINDEX, m_func);
        // call the function on top of the stack (throws exception on error)
        call(0);
        // return the value
        return detail::value_extractor<Ret>::get(m_vm);
    }

    template <typename T1>
    Ret operator()(const T1& p1)
    {
        lua_rawgeti(m_vm, LUA_REGISTRYINDEX, m_func);
        // push the argument and call with 1 arg
        detail::push_value(m_vm, p1);
        call(1);
        return detail::value_extractor<Ret>::get(m_vm);
    }

    template <typename T1, typename T2>
    Ret operator()(const T1& p1, const T2& p2)
    {
        lua_rawgeti(m_vm, LUA_REGISTRYINDEX, m_func);
        // push the arguments and call with 2 args
        detail::push_value(m_vm, p1);
        detail::push_value(m_vm, p2);
        call(2);
        return detail::value_extractor<Ret>::get(m_vm);
    }

    template <typename T1, typename T2, typename T3>
    Ret operator()(const T1& p1, const T2& p2, const T3& p3)
    {
        lua_rawgeti(m_vm, LUA_REGISTRYINDEX, m_func);
        detail::push_value(m_vm, p1);
        detail::push_value(m_vm, p2);
        detail::push_value(m_vm, p3);
        call(3);
        return detail::value_extractor<Ret>::get(m_vm);
    }

    // et cetera, provide as many overloads as you need
};

// we need to specialize the function for void return type
// as the other class would fail to compile with void as return type
template <>
class lua_function<void> : public lua_function_base
{
public:
    lua_function(lua_State *vm, const std::string& func)
        : lua_function_base(vm, func)
    {
    }

    void operator()()
    {
        lua_rawgeti(m_vm, LUA_REGISTRYINDEX, m_func);
        call(0);
    }

    template <typename T1>
    void operator()(const T1& p1)
    {
        lua_rawgeti(m_vm, LUA_REGISTRYINDEX, m_func);
        detail::push_value(m_vm, p1);
        call(1);
    }

    template <typename T1, typename T2>
    void operator()(const T1& p1, const T2& p2)
    {
        lua_rawgeti(m_vm, LUA_REGISTRYINDEX, m_func);
        detail::push_value(m_vm, p1);
        detail::push_value(m_vm, p2);
        call(2);
    }

    template <typename T1, typename T2, typename T3>
    void operator()(const T1& p1, const T2& p2, const T3& p3)
    {
        lua_rawgeti(m_vm, LUA_REGISTRYINDEX, m_func);
        detail::push_value(m_vm, p1);
        detail::push_value(m_vm, p2);
        detail::push_value(m_vm, p3);
        call(3);
    }

    // et cetera, provide as many overloads as you need
};

void
report_errors(lua_State *L, int status)
{
  if (status) {
    std::cerr << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1); // remove error message
  }
}

int
main(int argc, char** argv)
{
  const char *file = "test.lua";
  try {

    lua_State *L = lua_open();
    luaL_openlibs(L);

    int err = luaL_loadfile(L, file);
    if (!err)
      // execute Lua program
      err = lua_pcall(L, 0, LUA_MULTRET, 0);
#ifdef DEBUG
    if (err)
      report_errors(L, err);
#endif // DEBUG
    if (!err) {
      // See: http://stackoverflow.com/questions/7885299/c-call-lua-function-with-variable-parameters
      boost::function<int (std::string, std::string)> load = lua_function<int>(L, "load");
      std::string url("http://www.bilibili.tv/video/av205838/");
      std::string tmpfile ("tmp");
      int res = load(url, tmpfile);
      lua_getglobal(L, "g_mrls_size");
      int g_mrls_size = lua_tonumber(L, -1);
      if (g_mrls_size > 0) {
        lua_getglobal(L, "g_title");

        std::string g_title;
        lua_getglobal(L, "g_title");
        if (!lua_isnil(L, -1))
           g_title = lua_tostring(L, -1);

        std::string g_ref;
        lua_getglobal(L, "g_ref");
        if (!lua_isnil(L, -1))
          g_ref = lua_tostring(L, -1);

        std::string g_sub;
        lua_getglobal(L, "g_sub");
        if (!lua_isnil(L, -1))
          g_sub = lua_tostring(L, -1);

        std::list<std::string> g_mrls;
        lua_getglobal(L, "g_mrls");
        if (!lua_isnil(L, -1)) {
          lua_pushnil(L);
          while (lua_next(L, -2)) {
            if (lua_isstring(L, -1))
              g_mrls.push_back(lua_tostring(L, -1));
            lua_pop( L, 1 );
          }
          lua_pop( L, 1 );
        }

#ifdef DEBUG
        std::cout << "g_title: " << g_title << "\n"
                  << "g_ref: " << g_ref << "\n"
                  << "g_sub: " << g_sub << "\n"
                  << "g_mrls_size: " << g_mrls_size << "\n"
                  << "g_mrls: " << std::endl;
        for (std::list<std::string>::const_iterator p = g_mrls.begin();
             p != g_mrls.end(); ++p)
          std::cout << *p << std::endl;
#endif // DEBUG
      }
    }

    lua_close(L);
  } catch (std::exception &e) {
#ifdef DEBUG
    std::cerr << "exception: " << e.what() << std::endl;
#endif // DEBUG
  }

  return 0;
}
