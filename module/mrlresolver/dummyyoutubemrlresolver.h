#ifndef DUMMYYOUTUBEMRLRESOLVER_H
#define DUMMYYOUTUBEMRLRESOLVER_H

// dummygooglevideomrlresolver.h
// 2/2/2012

#include "module/mrlresolver/mrlresolver.h"

class DummyYoutubeMrlResolver : public MrlResolver
{
  Q_OBJECT
  Q_DISABLE_COPY(DummyYoutubeMrlResolver)
  typedef DummyYoutubeMrlResolver Self;
  typedef MrlResolver Base;

public:
  explicit DummyYoutubeMrlResolver(QObject *parent = nullptr)
    : Base(parent) { }

public:
  bool matchMedia(const QString &href) const override;

public slots:
  bool resolveMedia(const QString &href) override;
};

#endif // DUMMYYOUTUBEMRLRESOLVER_H
