#pragma once

// applocale.h
// 9/1/2012

#include <QtCore/QObject>

class AppLocale : public QObject
{
  Q_OBJECT
  typedef AppLocale Self;
  typedef QObject Base;

  int lcid_;
public:
  static bool createProcess(const QString &path, int icid);

  static bool isAvailable();

  static const char *downloadUrl()
  { return "http://www.microsoft.com/download/details.aspx?id=2043"; }

public:
  explicit AppLocale(QObject *parent = nullptr)
    : Base(parent), lcid_(0) { }

  bool createProcess(const QString &path)
  { return createProcess(path, lcid_); }

  int lcid() const { return lcid_; }
  void setLcid(int value) { lcid_ = value; }
};

// EOF
