// mrlanalysis.cc
// 4/11/2012

#include "mrlanalysis.h"

//#define DEBUG "mrlanalysis"
#include "module/debug/debug.h"

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
      ret = AcFun;
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
      ret = AcFun;
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
