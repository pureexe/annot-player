// luamrlresolver.cc
// 2/2/2012

#include "module/mrlresolver/luamrlresolver.h"
#include "module/mrlresolver/mrlresolversettings.h"
#include "module/qtext/htmltag.h"
#ifdef WITH_MODULE_LUARESOLVER
#  include "module/luaresolver/luaresolver.h"
#else
#  error "luaresolver module is required"
#endif // WITH_MODULE_LUARESOLVER
#ifdef WITH_MODULE_MRLANALYSIS
#  include "module/mrlanalysis/mrlanalysis.h"
#else
#  error "mrlanalysis module is required"
#endif // WITH_MODULE_MRLANALYSIS
#include <QtNetwork/QNetworkCookieJar>
#include <QtCore/QCoreApplication>
#include <QtCore/QRunnable>
#include <QtCore/QTextCodec>
#include <QtCore/QThreadPool>

#define DEBUG "luaemrlresolver"
#include "module/debug/debug.h"

// TODO: move to project source project instead of hard code here
#ifdef Q_OS_WIN
#  define LUA_PATH "lua/luascript" // omit applicationDirPath, which might contains non-ascii chars and cannot handled by liblua
#elif defined Q_OS_MAC
#  define LUA_PATH QCoreApplication::applicationDirPath() + "/lua"
#elif defined Q_WS_X11
#  define LUA_PATH LUADIR
#endif // Q_OS_

#ifdef LUA_PATH
#  define LUAPACKAGE_PATH LUA_PATH "/?.lua"
#  define LUASCRIPT_PATH  LUA_PATH "/luascript.lua"
#else
#  define LUAPACKAGE_PATH ""
#  define LUASCRIPT_PATH  "luascript.lua"
#endif // LUA_PATH

// - Tasks -

namespace { namespace task_ {

  class ResolveMedia : public QRunnable
  {
    LuaMrlResolver *r_;
    QString ref_;
    virtual void run() { r_->resolveMedia(ref_, false); } // \reimp, async = false
  public:
    ResolveMedia(const QString &ref, LuaMrlResolver *r)
      : r_(r), ref_(ref) { Q_ASSERT(r_); }
  };

  class ResolveSubtitle : public QRunnable
  {
    LuaMrlResolver *r_;
    QString ref_;
    virtual void run() { r_->resolveSubtitle(ref_, false); } // \reimp, async = false
  public:
    ResolveSubtitle(const QString &ref, LuaMrlResolver *r)
      : r_(r), ref_(ref) { Q_ASSERT(r_); }
  };

} } // anonymous namespace task_

// - Analysis -

bool
LuaMrlResolver::matchMedia(const QString &href) const
{
  DOUT("enter");
  int site;
  bool ret = (site = MrlAnalysis::matchSite(href)) &&
              site < MrlAnalysis::ChineseVideoSite;
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
LuaMrlResolver::matchSubtitle(const QString &href) const
{
  DOUT("enter");
  int site;
  bool ret = (site = MrlAnalysis::matchSite(href)) &&
              site < MrlAnalysis::AnnotationSite;
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
LuaMrlResolver::resolveMedia(const QString &href, bool async)
{
  if (async) {
    if (!checkSiteAccount(href))
      return false;
    QThreadPool::globalInstance()->start(new task_::ResolveMedia(href, this));
    return true;
  }
  if (!checkSiteAccount(href))
    return false;
  DOUT("enter: href =" << href);
  QString url = cleanUrl(href);
  DOUT("url =" << url);

  MrlResolverSettings *settings = MrlResolverSettings::globalSettings();

  //LuaResolver *lua = makeResolver();
  LuaResolver lua(LUASCRIPT_PATH, LUAPACKAGE_PATH);
  //lua.setNetworkAccessManager(networkAccessManager());
  //if (!lua.cookieJar())
  //  lua.setCookieJar(new QNetworkCookieJar);
  if (settings->hasNicovideoAccount())
    lua.setNicovideoAccount(settings->nicovideoAccount().username,
                            settings->nicovideoAccount().password);
  if (settings->hasBilibiliAccount())
    lua.setBilibiliAccount(settings->bilibiliAccount().username,
                           settings->bilibiliAccount().password);
  int siteid;
  QString refurl, title, suburl;
  QStringList mrls;
  QList<qint64> durations, sizes;
  bool ok = lua.resolve(url, &siteid, &refurl, &title, &suburl, &mrls, &durations, &sizes);
  if (!ok) {
    emit error(tr("failed to resolve URL") + ": " + url);
    DOUT("exit: LuaResolver returned false");
    return false;
  }

  if (mrls.isEmpty()) {
    switch (siteid) {
    case LuaResolver::Nicovideo:
      emit error(tr("failed to resolve URL using nicovideo account") + ": " + url);
      break;
    case LuaResolver::Bilibili:
      emit error(tr("failed to resolve URL using bilibili account") + ": " + url);
      break;
    default:
      emit error(tr("failed to resolve URL") + ": " + url);
    }
    DOUT("exit: mrls is empty");
    return false;
  }

  MediaInfo mi;
  mi.suburl = formatUrl(suburl);
  mi.refurl = formatUrl(refurl);
  mi.title = formatTitle(title);
  //switch (siteid) {
  //case LuaResolver::Acfun: encoding = "GBK"; break;
  ////case LuaResolver::Bilibili: encoding = "UTF-8"; break;
  //default: encoding = 0;
  //}

  switch (mrls.size()) { // Specialization for better performance
  case 0: break;
  case 1:
    {
      MrlInfo m(formatUrl(mrls.first()));
      if (!durations.isEmpty())
        m.duration = durations.front();
      if (!sizes.isEmpty())
        m.size = sizes.first();
      mi.mrls.append(m);
    } break;

  default:
    for (int i = 0; i < mrls.size(); i++) {
      MrlInfo m(formatUrl(mrls[i]));
      if (durations.size() > i)
        m.duration = durations[i];
      if (sizes.size() > i)
        m.size = sizes[i];
      mi.mrls.append(m);
    }
  }

  //QNetworkCookieJar *jar = lua.cookieJar();
  //Q_ASSERT(jar);
  //if (jar)
  //  jar->setParent(0);
  QNetworkCookieJar *jar = lua.cookieJar();
  if (jar)
    jar->setParent(0);

  emit mediaResolved(mi, jar);
  if (isSynchronized()) {
    setResolvedCookieJar(jar);
    setResolvedMediaInfo(mi);
  }
  DOUT("exit: title =" << mi.title);
  return true;
}

bool
LuaMrlResolver::resolveSubtitle(const QString &href, bool async)
{
  if (async) {
    if (!checkSiteAccount(href))
      return false;
    QThreadPool::globalInstance()->start(new task_::ResolveSubtitle(href, this));
    return true;
  }
  if (!checkSiteAccount(href))
    return false;
  DOUT("enter: href =" << href);
  QString url = cleanUrl(href);
  DOUT("url =" << url);

  MrlResolverSettings *settings = MrlResolverSettings::globalSettings();

  //LuaResolver *lua = makeResolver();
  LuaResolver lua(LUASCRIPT_PATH, LUAPACKAGE_PATH);
  //lua.setNetworkAccessManager(networkAccessManager());
  //lua.setCookieJar(new QNetworkCookieJar); // potential memory leak on error
  if (settings->hasNicovideoAccount())
    lua.setNicovideoAccount(settings->nicovideoAccount().username,
                            settings->nicovideoAccount().password);
  if (settings->hasBilibiliAccount())
    lua.setBilibiliAccount(settings->bilibiliAccount().username,
                           settings->bilibiliAccount().password);

  // LuaResolver::resolve(const QString &href, QString *refurl, QString *title, QString *suburl, QStringList *mrls) const
  int siteid;
  QString suburl;
  bool ok = lua.resolve(url, &siteid, 0, 0, &suburl);

  if (!ok) {
    emit error(tr("failed to resolve URL") + ": " + url);
    DOUT("exit: LuaResolver returned false");
    return false;
  }

  if (suburl.isEmpty()) {
    switch (siteid) {
    case LuaResolver::Nicovideo:
      emit error(tr("failed to resolve URL using nicovideo account") + ": " + url);
      break;
    case LuaResolver::Bilibili:
      emit error(tr("failed to resolve URL using bilibili account") + ": " + url);
      break;
    default:
      emit error(tr("failed to resolve URL") + ": " + url);
    }
    DOUT("exit: suburl is empty");
    return false;
  }

  suburl = formatUrl(suburl);
  if (!suburl.isEmpty())
    emit subtitleResolved(suburl);
  if (isSynchronized())
    setResolvedSubtitleUrl(suburl);
  DOUT("exit: suburl =" << suburl);
  return true;
}

// - Helper -

//LuaResolver*
//LuaMrlResolver::makeResolver(QObject *parent)
//{
//  LuaResolver *ret = new LuaResolver(LUASCRIPT_PATH, LUAPACKAGE_PATH, 0, parent);
//  if (hasNicovideoAccount())
//    ret->setNicovideoAccount(nicovideoUsername_, nicovideoPassword_);
//  if (hasBilibiliAccount())
//    ret->setBilibiliAccount(bilibiliUsername_, bilibiliPassword_);
//  return ret;
//}

QString
LuaMrlResolver::decodeText(const QString &text, const char *encoding)
{
  if (text.isEmpty())
    return QString();
  if (encoding) {
    QTextCodec *tc = QTextCodec::codecForName(encoding);
    if (tc) {
      QTextDecoder *dc = tc->makeDecoder();
      if (dc)
        return dc->toUnicode(text.toLocal8Bit());
    }
  }
  return text;
}

QString
LuaMrlResolver::formatTitle(const QString &title)
{
  return title.isEmpty() ? title : ::html_unescape(title)
    .remove(QRegExp("..ニコニコ動画\\(原宿\\)$"))
    .remove(QRegExp(" - 嗶哩嗶哩 - .*"))
    .remove(QRegExp(" - Acfun - .*"))
    .remove(QRegExp(" - AcFun.tv$"))
     // Youku
#ifdef _MSC_VER
    .remove(QRegExp(" - \xe7\x94\xb5\xe8\xa7\x86\xe5\x89\xa7 - .*"))
    .remove(QRegExp(" - \xe8\xa7\x86\xe9\xa2\x91 - .*"))
    .remove(QRegExp(" - \xe8\xa7\x86\xe9\xa2\x91$"))
    .remove(QRegExp(" - ..\xe8\xa7\x86\xe9\xa2\x91 - .*"))
    .remove(QRegExp(" - \xe5\x8a\xa8\xe6\xbc\xab$"))
    .remove(QRegExp("_\xe5\x9c\xa8\xe7\xba\xbf.*"))
#else
    .remove(QRegExp(" - 电视剧 - .*"))
    .remove(QRegExp(" - 视频 - .*"))
    .remove(QRegExp(" - 视频$"))
    .remove(QRegExp(" - 优酷视频 - .*")) // Youku
    .remove(QRegExp(" - 动漫$")) // Youku
    .remove(QRegExp("_在线.*")) // Tudou
#endif // _MSC_VER
    .remove(QRegExp(" - 20[01]\\d$")) // Youku
    .remove(QRegExp(" - 19[89]\\d$")) // Youku
     .simplified();
}

QString
LuaMrlResolver::formatUrl(const QString &href)
{
  QString ret = href.trimmed();
  return ret.isEmpty() ? ret : ret
    .remove(QRegExp("#$"))
    .remove(QRegExp("#titles$"))
    .replace(QRegExp("/index.html$", Qt::CaseInsensitive), "/")
    .replace(QRegExp("/#$", Qt::CaseInsensitive), "/")
    .replace(QRegExp("/index_1.html$", Qt::CaseInsensitive), "/")
    .replace(QRegExp("/default.html$", Qt::CaseInsensitive), "/");
}

QString
LuaMrlResolver::cleanUrl(const QString &url)
{
  QString ret = url.trimmed();
  ret.replace("http://acfun.tv/" , "http://www.acfun.tv", Qt::CaseInsensitive)
     .replace("http://bilibili.tv/" , "http://www.bilibili.tv", Qt::CaseInsensitive);
  if (ret.startsWith("http://www.nicovideo.jp/watch/", Qt::CaseInsensitive))
    ret.remove(QRegExp("\\?.*"));
  else if (ret.startsWith("http://www.bilibili.tv/video/", Qt::CaseInsensitive))
    ret.remove(QRegExp("/#$"))
       //.remove(QRegExp("/$"))
       .remove(QRegExp("/index_1.html$", Qt::CaseInsensitive))
       .remove(QRegExp("/index.html$", Qt::CaseInsensitive));
  else if (ret.startsWith("http://www.acfun.tv/v/", Qt::CaseInsensitive))
    ret.remove(QRegExp("/#$"))
       .remove(QRegExp("/$"))
       .remove(QRegExp("/index.html$", Qt::CaseInsensitive));
  else if (ret.startsWith("http://www.tudou.com/programs/view/", Qt::CaseInsensitive))
    ret.remove(QRegExp("\\?.*"));
  return ret;
}

bool
LuaMrlResolver::checkSiteAccount(const QString &href)
{
  DOUT("enter");
  if (href.contains("nicovideo.jp/", Qt::CaseInsensitive)
      && !MrlResolverSettings::globalSettings()->hasNicovideoAccount()) {
    emit error(tr("nicovideo.jp account is required to resolve URL") + ": " + href);
    DOUT("exit: ret = false, nico account required");
    return false;
  }
  DOUT("exit: ret = true");
  return true;
}

// EOF
