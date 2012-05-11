// searchenginefactory.cc
// 4/25/2012
#include "module/searchengine/searchenginefactory.h"
#include "module/searchengine/searchenginerc.h"

// FIXME: character encoding is incorrect.

SearchEngine*
SearchEngineFactory::create(int engine) const
{
  switch (engine) {
  case Google:    return new SearchEngine(Google,   SE_GOOGLE,   tr("Google"),   WBRC_IMAGE_GOOGLE,   QStringList("g") << "gg" << "google");// << "ぐぐ" << "ググ" << "谷歌"
  case GoogleImages:return new SearchEngine(GoogleImages, SE_GOOGLE_IMAGES, tr("Google Images"), WBRC_IMAGE_GOOGLE,   QStringList("image") << "images" << "画像" << "图片");
  case Bing:      return new SearchEngine(Bing,     SE_BING,     "Bing",         WBRC_IMAGE_BING,     QStringList("bing"));

  case Youtube:   return new SearchEngine(Youtube,  SE_YOUTUBE,  tr("Youtube"),  WBRC_IMAGE_YOUTUBE,  QStringList("y") << "youtube" << "yt" << "ヨウツベ" << "视频");
  case Nicovideo: return new SearchEngine(Nicovideo,SE_NICOVIDEO,tr("Nicovideo"),WBRC_IMAGE_NICOVIDEO,QStringList("n") << "nico" << "nicovideo" << "anime" << "にこ" << "ニコ" << "ニコニコ" << "ニコ動" << "アニメ");
  case Bilibili:  return new SearchEngine(Bilibili, SE_BILIBILI, tr("Bilibili"), WBRC_IMAGE_BILIBILI, QStringList("b") << "bili" << "bilibili" << "ビリビリ" << "哔哩哔哩" << "新番");
  case Acfun:     return new SearchEngine(Acfun,    SE_ACFUN,    tr("AcFun"),    WBRC_IMAGE_ACFUN,    QStringList("a") << "ac" << "acfun");
  case Youku:     return new SearchEngine(Youku,    SE_YOUKU,    tr("Youku"),    WBRC_IMAGE_YOUKU,    QStringList("youku") << "ヨウク" << "优酷");

  case WikiJa:    return new SearchEngine(WikiJa,   SE_WIKI_JA,  tr("Wikipedia (ja)"), WBRC_IMAGE_WIKI, QStringList("ja") << "j" << "ウィキ" << "百科");
  case WikiEn:    return new SearchEngine(WikiEn,   SE_WIKI_EN,  tr("Wikipedia (en)"), WBRC_IMAGE_WIKI, QStringList("en") << "w" << "wiki" << "wikipedia");
  case WikiZh:    return new SearchEngine(WikiZh,   SE_WIKI_ZH,  tr("Wikipedia (zh)"), WBRC_IMAGE_WIKI, QStringList("zh") << "维基");
  case Manpage:   return new SearchEngine(Manpage,  SE_MAN,      "Manpages",     "",                  QStringList("man"));
  case CPlusPlus: return new SearchEngine(CPlusPlus,SE_CPP,      "C++",          WBRC_IMAGE_CPP,      QStringList("cpp") << "c++" << "cplusplus");
  case Qt:        return new SearchEngine(Qt,       SE_QT,       "Qt",           WBRC_IMAGE_QT,       QStringList("qt"));
  case Osx:       return new SearchEngine(Osx,      SE_OSX,      "OS X",         WBRC_IMAGE_APPLE,    QStringList("osx") << "objc");
  case Msdn:      return new SearchEngine(Msdn,     SE_MSDN,     "MSDN",         WBRC_IMAGE_MSDN,     QStringList("msdn"));
  case Bash:      return new SearchEngine(Bash,     SE_BASH,     "Bash",         WBRC_IMAGE_SS64,     QStringList("bash"));
  case Cmd:       return new SearchEngine(Cmd,      SE_CMD,      "Cmd",          WBRC_IMAGE_SS64,     QStringList("cmd"));
  default: Q_ASSERT(0); return 0;
  }
}

// EOF
