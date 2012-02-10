#ifndef LUAMRLRESOLVER_H
#define LUAMRLRESOLVER_H

// luamrlresolver.h
// 1/24/2011

#include "mrlresolver.h"
#include <QString>
#include <string>

//QT_FORWARD_DECLARE_CLASS(QTextDecoder)

class LuaMrlResolver : public MrlResolver
{
  Q_OBJECT
  typedef LuaMrlResolver Self;
  typedef MrlResolver Base;

  QString nicovideoUsername_, nicovideoPassword_;
  QString bilibiliUsername_, bilibiliPassword_;

public:
  explicit LuaMrlResolver(QObject *parent = 0)
    : Base(parent) { }

public:
  bool matchMedia(const QString &href) const; ///< \override
  bool matchSubtitle(const QString &href) const; ///< \override

public slots:
  void resolveMedia(const QString &href) ///< \override
  { resolveMedia(href, true); }

  void resolveMedia(const QString &href, bool async);

  void resolveSubtitle(const QString &href) ///< \override
  { resolveSubtitle(href, true); }

  void resolveSubtitle(const QString &href, bool async);

public:
  const QString &nicovideoUsername() const { return nicovideoUsername_; }
  const QString &nicovideoPassword() const { return nicovideoPassword_; }
  const QString &bilibiliUsername() const { return bilibiliUsername_; }
  const QString &bilibiliPassword() const { return bilibiliPassword_; }

  bool hasNicovideoAccount() const
  { return !nicovideoUsername_.isEmpty() && !nicovideoPassword_.isEmpty(); }

  bool hasBilibiliAccount() const
  { return !bilibiliUsername_.isEmpty() && !bilibiliPassword_.isEmpty(); }

public slots:
  void setNicovideoAccount(const QString &username, const QString &password)
  { nicovideoUsername_ = username; nicovideoPassword_ = password; }

  void clearNicovideoAccount()
  { nicovideoUsername_.clear(); nicovideoPassword_.clear(); }

  void setBilibiliAccount(const QString &username, const QString &password)
  { bilibiliUsername_ = username; bilibiliPassword_ = password; }

  void clearBilibiliAccount()
  { bilibiliUsername_.clear(); bilibiliPassword_.clear(); }

protected:
  //LuaResolver *makeResolver(QObject *parent = 0);

  ///  Return true if succeed.
  bool checkSiteAccount(const QString &href);

  // It's important to keep these methods stateless.
  static QString decodeText(const QString &text, const char *encoding = 0);
  static QString formatTitle(const QString &title, const char *encoding = 0);
  static QString formatUrl(const QString &url);

  static QString cleanUrl(const QString &url);
};

#endif // LUAMRLRESOLVER_H
