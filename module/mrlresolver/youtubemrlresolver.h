#ifndef YOUTUBEMRLRESOLVER_H
#define YOUTUBEMRLRESOLVER_H

// Youtubemrlresolver.h
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
  bool match(const QString &href) const; ///< \override

public slots:
  void resolve(const QString &href); ///< \override
};

#endif // YOUTUBEMRLRESOLVER_H
