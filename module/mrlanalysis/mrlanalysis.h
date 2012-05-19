#ifndef MRLANALYSIS_H
#define MRLANALYSIS_H

// mrlanalysis.h
// 4/11/2012

#include <QtCore/QString>

// Video sites with annotations
#define MA_EIGEN_NICOVIDEO      "www.nicovideo.jp/watch/"
#define MA_EIGEN_BILIBILI       "www.bilibili.tv/video/av"
#define MA_EIGEN_ACFUN          "www.acfun.tv/v/ac"

// Video sites from China
#define MA_EIGEN_YOUKU          "v.youku.com/v_"
//#define MA_EIGEN_TUDOU          "www.tudou.com/p"
#define MA_EIGEN_TUDOU          "www.tudou.com/programs/view/"
#define MA_EIGEN_SINAVIDEO      "video.sina.com.cn/v/"
#define MA_EIGEN_QQVIDEO        "v.qq.com/"

// Video sites
#define MA_EIGEN_YOUTUBE        "www.youtube.com/watch?"
#define MA_EIGEN_GOOGLEVIDEO    "video.google.com/videoplay"

namespace MrlAnalysis
{
  enum Site { NoSite = 0,
    Nicovideo, Bilibili, Acfun, AnnotationSite,
    Youku, Tudou, SinaVideo, QQVideo, ChineseVideoSite,
    Youtube, GoogleVideo, VideoSite
  };

  Site matchSite(const QString &url, bool href = true);
}

#endif // MRLANALYSIS_H
