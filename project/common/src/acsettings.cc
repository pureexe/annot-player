// acsettings.cc
// 7/30/2011

#include "ac/acsettings.h"
#include "ac/acglobal.h"
#include "ac/acui.h"
#include "module/crypt/crypt.h"
#include "module/crypt/simplecrypt.h"
#include "module/qtext/algorithm.h"
#include <QtNetwork/QNetworkProxy>

// - AcSettings keys -

// See platform specific issue in QAcSettings manual.
#ifdef Q_OS_MAC
  #define SK_ORGANIZATION   AC_DOMAIN
#else
  #define SK_ORGANIZATION   AC_ORGANIZATION
#endif // Q_OS_MAC

#define SK_APPLICATION    AC_APPLICATION
#define SK_VERSION        "Version"

#define SK_USERID       "UserId"
#define SK_USERNAME     "UserName"
#define SK_PASSWORD     "Password"
#define SK_NICOACCOUNT  "NicovideoAccount"
#define SK_BILIACCOUNT  "BilibiliAccount"

#define SK_LANGUAGE     "Language"

#define SK_THEME        "Theme"
#define SK_AERO         "Aero"
#define SK_MENUTHEME    "MenuTheme"

#define SK_PROXY_ENABLED "ProxyEnabled"
#define SK_PROXY_HOST   "ProxyHost"
#define SK_PROXY_PORT   "ProxyPort"
#define SK_PROXY_TYPE   "ProxyType"
#define SK_PROXY_USER   "ProxyUser"
#define SK_PROXY_PASS   "ProxyPassword"

// - Constructions -

AcSettings::AcSettings(QObject *parent)
  : Base(
      Base::NativeFormat, Base::UserScope,
      SK_ORGANIZATION, SK_APPLICATION,
      parent)
{ }

void
AcSettings::sync()
{
  QMutexLocker lock(&m_);
  Base::sync();
}

bool
AcSettings::fackCcp() const
{ return language() == QLocale::Chinese; }

// - Settings -

QString
AcSettings::version() const
{ return value(SK_VERSION).toString(); }

void
AcSettings::setVersion(const QString &version)
{ setValue(SK_VERSION, version); }

// - Accounts -

qint64
AcSettings::userId() const
{ return value(SK_USERID).toLongLong(); }

void
AcSettings::setUserId(qint64 uid)
{ setValue(SK_USERID, uid); }

QString
AcSettings::userName() const
{
  QString ret = value(SK_USERNAME).toString();
  return ret.isEmpty() ? ret : Crypt::decrypt(ret);
}

void
AcSettings::setUserName(const QString &userName)
{
  if (userName.isEmpty())
    remove(SK_USERNAME);
  else
    setValue(SK_USERNAME, Crypt::encrypt(userName));
}

QString
AcSettings::password() const
{
  QString ret = value(SK_PASSWORD).toString();
  return ret.isEmpty() ? ret : Crypt::decrypt(ret);
}

void
AcSettings::setPassword(const QString &password)
{
  if (password.isEmpty())
    remove(SK_PASSWORD);
  else
    setValue(SK_PASSWORD, Crypt::encrypt(password));
}

std::pair<QString, QString>
AcSettings::nicovideoAccount()
{
  QString username, password;
  QStringList v = value(SK_NICOACCOUNT).toStringList();
  if (v.size() == 2) {
    SimpleCrypt c(0);
    username = c.decryptToString(v.first());
    password = c.decryptToString(v.last());
  }
  return std::make_pair(username, password);
}

void
AcSettings::setNicovideoAccount(const QString &username, const QString &password)
{
  if (username.isEmpty() || password.isEmpty())
    remove(SK_NICOACCOUNT);
  else {
    SimpleCrypt c(0);
    QStringList v;
    v.append(c.encryptToString(username));
    v.append(c.encryptToString(password));
    setValue(SK_NICOACCOUNT, v);
  }
}

std::pair<QString, QString>
AcSettings::bilibiliAccount()
{
  QString username, password;
  QStringList v = value(SK_BILIACCOUNT).toStringList();
  if (v.size() == 2) {
    SimpleCrypt c(0);
    username = c.decryptToString(v.first());
    password = c.decryptToString(v.last());
  }
  return std::make_pair(username, password);
}

void
AcSettings::setBilibiliAccount(const QString &username, const QString &password)
{
  if (username.isEmpty() || password.isEmpty()) {
    remove(SK_BILIACCOUNT);
  } else {
    SimpleCrypt c(0);
    QStringList v;
    v.append(c.encryptToString(username));
    v.append(c.encryptToString(password));
    setValue(SK_BILIACCOUNT, v);
  }
}

// - i18n -

int
AcSettings::language() const
{ return value(SK_LANGUAGE).toInt(); }

void
AcSettings::setLanguage(int language)
{ setValue(SK_LANGUAGE, language); }

// - GUI -

void
AcSettings::setThemeId(int tid)
{ setValue(SK_THEME, tid); }

int
AcSettings::themeId() const
{
  enum { defval = AcUi::RandomTheme };
  return value(SK_THEME, defval).toInt();
}

bool
AcSettings::isAeroEnabled() const
{ return value(SK_AERO, true).toBool(); }

void
AcSettings::setAeroEnabled(bool t)
{ setValue(SK_AERO, t); }

bool
AcSettings::isMenuThemeEnabled() const
{ return value(SK_MENUTHEME).toBool(); }

void
AcSettings::setMenuThemeEnabled(bool t)
{ setValue(SK_MENUTHEME, t); }


// - Network proxy -

void
AcSettings::setProxyEnabled(bool t)
{ setValue(SK_PROXY_ENABLED, t); }

bool
AcSettings::isProxyEnabled() const
{ return value(SK_PROXY_ENABLED).toBool(); }

void
AcSettings::setProxyHostName(const QString &host)
{ setValue(SK_PROXY_HOST, host); }

QString
AcSettings::proxyHostName() const
{ return value(SK_PROXY_HOST).toString(); }

void
AcSettings::setProxyPort(int port)
{ setValue(SK_PROXY_PORT, port); }

int
AcSettings::proxyPort() const
{ return value(SK_PROXY_PORT).toInt(); }

void
AcSettings::setProxyType(int type)
{ setValue(SK_PROXY_TYPE, type); }

int
AcSettings::proxyType() const
{
  enum { defval = QNetworkProxy::Socks5Proxy };
  return value(SK_PROXY_TYPE, defval).toInt();
}

QString
AcSettings::proxyUser() const
{
  QString ret = value(SK_PROXY_USER).toString();
  return ret.isEmpty() ? ret : Crypt::decrypt(ret);
}

void
AcSettings::setProxyUser(const QString &userName)
{
  if (userName.isEmpty())
    remove(SK_PROXY_USER);
  else
    setValue(SK_PROXY_USER, Crypt::encrypt(userName));
}

QString
AcSettings::proxyPassword() const
{
  QString ret = value(SK_PROXY_PASS).toString();
  return ret.isEmpty() ? ret : Crypt::decrypt(ret);
}

void
AcSettings::setProxyPassword(const QString &password)
{
  if (password.isEmpty())
    remove(SK_PROXY_PASS);
  else
    setValue(SK_PROXY_PASS, Crypt::encrypt(password));
}

// EOF
