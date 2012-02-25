#ifndef YOUTUBEMRLRESOLVER_H
#define YOUTUBEMRLRESOLVER_H

// youtubemrlresolver.h
// 1/24/2011

#include "mrlresolver.h"

class YoutubeMrlResolver : public MrlResolver
{
  Q_OBJECT
  typedef YoutubeMrlResolver Self;
  typedef MrlResolver Base;

public:
  explicit YoutubeMrlResolver(QObject *parent = 0)
    : Base(parent) { }

public:
  bool matchMedia(const QString &href) const; ///< \override

public slots:
  bool resolveMedia(const QString &href); ///< \override
};

#endif // YOUTUBEMRLRESOLVER_H
