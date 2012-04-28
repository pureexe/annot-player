#ifndef GOOGLEVIDEOMRLRESOLVER_H
#define GOOGLEVIDEOMRLRESOLVER_H

// googlevideomrlresolver.h
// 2/2/2012

#include "module/mrlresolver/mrlresolver.h"

class GoogleVideoMrlResolver : public MrlResolver
{
  Q_OBJECT
  typedef GoogleVideoMrlResolver Self;
  typedef MrlResolver Base;

public:
  explicit GoogleVideoMrlResolver(QObject *parent = 0)
    : Base(parent) { }

public:
  bool matchMedia(const QString &href) const; ///< \override

public slots:
  bool resolveMedia(const QString &href); ///< \override
};

#endif // GOOGLEVIDEOMRLRESOLVER_H
