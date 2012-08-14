// mrlanalysis.cc
// 4/11/2012

#include "module/mrlanalysis/mrlanalysis.h"
#include <QtCore/QRegExp>

//#define DEBUG "mrlanalysis"
#include "module/debug/debug.h"

QString
MrlAnalysis::normalizeUrl(const QString &url)
{
  QString ret = url.trimmed();
  if (ret.isEmpty())
    return ret;
  if (ret.startsWith("http://www.nicovideo.jp/watch/"))
    ret.remove(QRegExp("\\?.*"));
  else if (ret.startsWith("http://nicovideo.jp/")) {
    ret.replace("http://nicovideo.jp/", "http://www.nicovideo.jp/");
    ret.remove(QRegExp("\\?.*"));
  } else if (ret.startsWith("http://bilibili.tv/"))
    ret.replace("http://bilibili.tv/", "http://www.bilibili.tv/");
  else if (ret.startsWith("http://acfun.tv/"))
    ret.replace("http://acfun.tv/", "http://www.acfun.tv/");
  else if (ret.startsWith("http://www.tudou.com/programs/view/", Qt::CaseInsensitive))
    ret.remove(QRegExp("\\?.*"));

  return ret
     .remove(QRegExp("/#.*$"))
     .remove(QRegExp("/index.html$", Qt::CaseInsensitive))
     .remove(QRegExp("/default.html$", Qt::CaseInsensitive))
     .remove(QRegExp("/index_1.html$", Qt::CaseInsensitive))
     .remove(QRegExp("/$"));
}

MrlAnalysis::Site
MrlAnalysis::matchSite(const QString &url, bool href)
{
  Site ret = NoSite;
  if (url.isEmpty())
    return ret;
  DOUT("enter: href =" << href << ", url =" << url);
  if (href) {
    if (!url.startsWith("http://", Qt::CaseInsensitive))
      ret = NoSite;
    else if (url.startsWith("http://" MA_EIGEN_NICOVIDEO, Qt::CaseInsensitive))
      ret = Nicovideo;
    else if (url.startsWith("http://" MA_EIGEN_BILIBILI, Qt::CaseInsensitive))
      ret = Bilibili;
    else if (url.startsWith("http://" MA_EIGEN_ACFUN, Qt::CaseInsensitive))
      ret = Acfun;
    else if (url.startsWith("http://" MA_EIGEN_YOUKU, Qt::CaseInsensitive))
      ret = Youku;
    else if (url.startsWith("http://" MA_EIGEN_SINAVIDEO, Qt::CaseInsensitive))
      ret = SinaVideo;
    else if (url.startsWith("http://" MA_EIGEN_QQVIDEO, Qt::CaseInsensitive))
      ret = QQVideo;
    else if (url.startsWith("http://" MA_EIGEN_TUDOU, Qt::CaseInsensitive))
      ret = Tudou;
    else if (url.startsWith("http://" MA_EIGEN_YOUTUBE, Qt::CaseInsensitive))
      ret = Youtube;
    else if (url.startsWith("http://" MA_EIGEN_GOOGLEVIDEO, Qt::CaseInsensitive))
      ret = GoogleVideo;
  } else {
    if (url.contains(MA_EIGEN_NICOVIDEO, Qt::CaseInsensitive))
      ret = Nicovideo;
    else if (url.contains(MA_EIGEN_BILIBILI, Qt::CaseInsensitive))
      ret = Bilibili;
    else if (url.contains(MA_EIGEN_ACFUN, Qt::CaseInsensitive))
      ret = Acfun;
    else if (url.contains(MA_EIGEN_YOUKU, Qt::CaseInsensitive))
      ret = Youku;
    else if (url.contains(MA_EIGEN_SINAVIDEO, Qt::CaseInsensitive))
      ret = SinaVideo;
    else if (url.contains(MA_EIGEN_YOUTUBE, Qt::CaseInsensitive))
      ret = Youtube;
    else if (url.contains(MA_EIGEN_GOOGLEVIDEO, Qt::CaseInsensitive))
      ret = GoogleVideo;
    else if (url.contains(MA_EIGEN_QQVIDEO, Qt::CaseInsensitive))
      ret = QQVideo;
    else if (url.contains(MA_EIGEN_TUDOU, Qt::CaseInsensitive))
      ret = Tudou;
  }
  DOUT("exit: ret =" << ret);
  return ret;
}

// EOF
