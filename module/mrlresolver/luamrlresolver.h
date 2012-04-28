#ifndef LUAMRLRESOLVER_H
#define LUAMRLRESOLVER_H

// luamrlresolver.h
// 1/24/2011

#include "module/mrlresolver/mrlresolver.h"
#include <QtCore/QString>

//QT_FORWARD_DECLARE_CLASS(QTextDecoder)

class LuaMrlResolver : public MrlResolver
{
  Q_OBJECT
  typedef LuaMrlResolver Self;
  typedef MrlResolver Base;

public:
  explicit LuaMrlResolver(QObject *parent = 0)
    : Base(parent) { }

public:
  bool matchMedia(const QString &href) const; ///< \override
  bool matchSubtitle(const QString &href) const; ///< \override

public slots:
  bool resolveMedia(const QString &href) ///< \override
  { return resolveMedia(href, true); }

  bool resolveMedia(const QString &href, bool async);

  bool resolveSubtitle(const QString &href) ///< \override
  { return resolveSubtitle(href, true); }

  bool resolveSubtitle(const QString &href, bool async);

protected:
  //LuaResolver *makeResolver(QObject *parent = 0);

  ///  Return true if succeed.
  bool checkSiteAccount(const QString &href);

  // It's important to keep these methods stateless.
  static QString decodeText(const QString &text, const char *encoding = 0);
  static QString formatTitle(const QString &title);
  static QString formatUrl(const QString &url);

  static QString cleanUrl(const QString &url);
};

#endif // LUAMRLRESOLVER_H
