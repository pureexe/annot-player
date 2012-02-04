// luamrlresolver.cc
// 2/2/2012

#include "luamrlresolver.h"
#include "luascript.h"
#include <boost/foreach.hpp>
#include <QtCore>
#include <QtScript>
#include <QtNetwork>
#include <string>

//#define DEBUG "luaemrlresolver"
#include "module/debug/debug.h"

// - Helper -

namespace { namespace task_ {

  class ResolveMedia : public QRunnable
  {
    LuaMrlResolver *r_;
    QString ref_;
    virtual void run() { r_->resolveMedia(ref_, false); } // \override, async = false
  public:
    ResolveMedia(const QString &ref, LuaMrlResolver *r)
      : ref_(ref), r_(r) { Q_ASSERT(r_); }
  };

  class ResolveAnnot : public QRunnable
  {
    LuaMrlResolver *r_;
    QString ref_;
    virtual void run() { r_->resolveAnnot(ref_, false); } // \override, async = false
  public:
    ResolveAnnot(const QString &ref, LuaMrlResolver *r)
      : ref_(ref), r_(r) { Q_ASSERT(r_); }
  };

} } // anonymous namespace task_

// - Analysis -

bool
LuaMrlResolver::match(const QString &href) const
{
  return href.startsWith("http://", Qt::CaseInsensitive) &&
  (
    href.contains("bilibili.tv/", Qt::CaseInsensitive) ||
    href.contains("acfun.tv/", Qt::CaseInsensitive) ||
    href.contains("youku.com/", Qt::CaseInsensitive) ||
    href.contains("video.sina.com.cn/", Qt::CaseInsensitive) ||
    href.contains("tudou.com/", Qt::CaseInsensitive) ||
    href.contains("mikufans.cn/", Qt::CaseInsensitive)
    //href.contains("nicovideo.jp/") ||
  );
}

void
LuaMrlResolver::resolveMedia(const QString &href, bool async)
{
  if (async) {
    QThreadPool::globalInstance()->start(new task_::ResolveMedia(href, this));
    return;
  }
  luascript::media_description md;
  bool ok = luascript::resolve_media(href.toStdString(), md);
  if (!ok) {
    emit errorReceived(tr("failed to resolve URL") + ": " + href);
    return;
  }

  MediaInfo mi;
  const char *encoding = 0;
  mi.suburl = formatUrl(md.suburl);
  mi.refurl = formatUrl(md.refurl);
  if (mi.refurl.contains("acfun.tv", Qt::CaseInsensitive))
    encoding = "GBK";
  mi.title = formatTitle(md.title, encoding);

  switch (md.mrls.size()) { // Specialization for better performance
  case 0: break;
  case 1: mi.mrls.append(formatUrl(md.mrls.front())); break;
  default:
    BOOST_FOREACH(std::string mrl, md.mrls)
      mi.mrls.append(formatUrl(mrl));
  }
  emit mediaResolved(mi);
}

void
LuaMrlResolver::resolveAnnot(const QString &href, bool async)
{
  if (async) {
    QThreadPool::globalInstance()->start(new task_::ResolveAnnot(href, this));
    return;
  }
  std::string suburl;
  bool ok = luascript::resolve_annot(href.toStdString(), suburl);
  if (!ok || suburl.empty()) {
    emit errorReceived(tr("failed to resolve URL") + ": " + href);
    return;
  }

  QString url = formatUrl(suburl);
  if (!url.isEmpty())
    emit annotResolved(url);
}

// - Helper -

QString
LuaMrlResolver::decodeText(const std::string &text, const char *encoding)
{
  if (text.empty())
    return QString();
  if (encoding) {
    QTextCodec *tc = QTextCodec::codecForName(encoding);
    if (tc) {
      QTextDecoder *dc = tc->makeDecoder();
      if (dc)
        return dc->toUnicode(text.c_str());
    }
  }
  return QString::fromStdString(text);
}

QString
LuaMrlResolver::formatTitle(const std::string &title, const char *encoding)
{
  QString ret = decodeText(title, encoding).trimmed();
  if (ret.isEmpty())
    return ret;

  ret = ret.remove(QRegExp(" - 嗶哩嗶哩 - .*"));
  ret = ret.remove(QRegExp(" - AcFun.tv$"));
  return ret;
}

QString
LuaMrlResolver::formatUrl(const std::string &href)
{
  QString ret = QString::fromStdString(href).trimmed();
  if (ret.isEmpty())
    return ret;

  //ret = ret.replace("http://www.", "http://", Qt::CaseInsensitive);
  //ret = ret.remove(QRegExp("/$"));
  return ret;
}

// EOF
