#ifndef LUAMRLRESOLVER_H
#define LUAMRLRESOLVER_H

// luamrlresolver.h
// 1/24/2011

#include "mrlresolver.h"
#include <string>

class LuaMrlResolver : public MrlResolver
{
  Q_OBJECT
  typedef LuaMrlResolver Self;
  typedef MrlResolver Base;

public:
  explicit LuaMrlResolver(QObject *parent = 0)
    : Base(parent) { }

public:
  bool match(const QString &href) const; ///< \override

public slots:
  void resolveMedia(const QString &href) ///< \override
  { resolveMedia(href, true); }

  void resolveMedia(const QString &href, bool async);

  void resolveAnnot(const QString &href) ///< \override
  { resolveAnnot(href, true); }

  void resolveAnnot(const QString &href, bool async);

protected:
  static QString decodeText(const std::string &text, const char *encoding = 0);
  static QString formatTitle(const std::string &title, const char *encoding = 0);
  static QString formatUrl(const std::string &url);
};

#endif // LUAMRLRESOLVER_H
