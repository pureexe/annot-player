#ifndef LUASCRIPT_H
#define LUASCRIPT_H

// luascript.h
// 2/2/2012
#include <string>
#include <list>

struct luascript
{
  struct media_description
  {
    std::string title;
    std::string refurl;
    std::string suburl;
    std::list<std::string> mrls;
  };

  static bool resolve_media(const std::string &href, media_description &md);
  static bool resolve_annot(const std::string &href, std::string &suburl);

protected:
  static std::string mktemp();
};

#endif // LUAMRLRESOLVER_H
