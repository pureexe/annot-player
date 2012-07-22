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
  explicit DummyGoogleVideoMrlResolver(QObject *parent = nullptr)
    : Base(parent) { }

public:
  bool matchMedia(const QString &href) const override;

public slots:
  bool resolveMedia(const QString &href) override;
};

#endif // DUMMYGOOGLEVIDEOMRLRESOLVER_H
