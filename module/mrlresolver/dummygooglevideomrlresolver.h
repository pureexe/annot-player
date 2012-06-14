#ifndef DUMMYGOOGLEVIDEOMRLRESOLVER_H
#define DUMMYGOOGLEVIDEOMRLRESOLVER_H

// dummygooglevideomrlresolver.h
// 2/2/2012

#include "module/mrlresolver/mrlresolver.h"

class DummyGoogleVideoMrlResolver : public MrlResolver
{
  Q_OBJECT
  Q_DISABLE_COPY(DummyGoogleVideoMrlResolver)
  typedef DummyGoogleVideoMrlResolver Self;
  typedef MrlResolver Base;

public:
  explicit DummyGoogleVideoMrlResolver(QObject *parent = 0)
    : Base(parent) { }

public:
  bool matchMedia(const QString &href) const; ///< \reimp

public slots:
  bool resolveMedia(const QString &href); ///< \reimp
};

#endif // DUMMYGOOGLEVIDEOMRLRESOLVER_H
