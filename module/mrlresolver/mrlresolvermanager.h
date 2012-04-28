#ifndef MRLRESOLVERMANAGER_H
#define MRLRESOLVERMANAGER_H

// mrlresolvermanager.h
// 1/25/2011

#include "module/mrlresolver/mrlresolver.h"
#include <QtCore/QList>
#include <QtCore/QObject>

class MrlResolverManager : public MrlResolver
{
  Q_OBJECT
  typedef MrlResolverManager Self;
  typedef MrlResolver Base;

  QList<MrlResolver*> resolvers_;

  // - Construction -
public:
  enum Resolver { Youtube = 0, GoogleVideo, Lua, ResolverCount };

  explicit MrlResolverManager(QObject *parent = 0);

  // - Analysis -
public:
  int resolverForMedia(const QString &href) const;
  int resolverForSubtitle(const QString &href) const;

  virtual bool matchMedia(const QString &href) const ///< \override
  { return resolverForMedia(href) >= 0; }
  virtual bool matchSubtitle(const QString &href) const ///< \override
  { return resolverForSubtitle(href) >= 0; }

  bool resolveMedia(int id, const QString &href);

  virtual bool resolveMedia(const QString &href) ///< \override
  {
    QString url = autoCompleteUrl(href);
    int r = resolverForMedia(url);
    return r >= 0 && resolveMedia(r, url);
  }

  bool resolveSubtitle(int id, const QString &href);
  virtual bool resolveSubtitle(const QString &href) ///< \override
  {
    QString url = autoCompleteUrl(href);
    int r = resolverForSubtitle(url);
    return r == Lua && resolveSubtitle(r, url);
  }

protected:
  static QString autoCompleteUrl(const QString &url);
};

#endif // MRLRESOLVERMANAGER_H
