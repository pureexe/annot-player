#ifndef LUARESOLVER_H
#define LUARESOLVER_H

// luaresolver.h
// 2/2/2012
#include <string>
#include <list>

struct lua_State;

class luaresolver
{
  std::string script_path_;
  std::string package_path_;

public:
  struct media_description
  {
    std::string title;
    std::string refurl;
    std::string suburl;
    std::list<std::string> mrls;
  };

  explicit luaresolver(const std::string &script_path, const std::string &package_path = std::string())
    : script_path_(script_path), package_path_(package_path) { }

public:
  const std::string &script_path() const { return script_path_; }
  const std::string &package_path() const { return package_path_; }

  bool resolve_media(const std::string &href, media_description &md) const;
  bool resolve_annot(const std::string &href, std::string &suburl) const;

protected:
  static std::string mktemp();
  static void print_last_error(lua_State *L);
  static void append_lua_path(lua_State *L, const char *path);

  void append_lua_path(lua_State *L) const
  {
    if (!package_path_.empty())
      append_lua_path(L, package_path_.c_str());
  }
};

#endif // LUARESOLVER_H
