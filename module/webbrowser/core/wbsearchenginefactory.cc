// wbsearchenginefactory.cc
// 4/25/2012
#include "core/wbsearchenginefactory.h"
#include "global/wbrc.h"

WbSearchEngine*
WbSearchEngineFactory::create(int engine) const
{
  switch (engine) {
  case Google:    return new WbSearchEngine(Google,   WBSE_GOOGLE,   tr("Google"),   WBRC_IMAGE_GOOGLE,   QStringList("g") << "gg" << "google");// << "ぐぐ" << "ググ" << "谷歌"
  case Youtube:   return new WbSearchEngine(Youtube,  WBSE_YOUTUBE,  tr("Youtube"),  WBRC_IMAGE_YOUTUBE,  QStringList("y") << "youtube" << "yt" << "ヨウツベ" << "视频");
  case Nicovideo: return new WbSearchEngine(Nicovideo,WBSE_NICOVIDEO,tr("Nicovideo"),WBRC_IMAGE_NICOVIDEO,QStringList("n") << "nico" << "nicovideo" << "anime" << "にこ" << "ニコ" << "ニコニコ" << "ニコ動" << "アニメ");
  case Bilibili:  return new WbSearchEngine(Bilibili, WBSE_BILIBILI, tr("Bilibili"), WBRC_IMAGE_BILIBILI, QStringList("b") << "bili" << "bilibili" << "ビリビリ" << "哔哩哔哩" << "新番");
  case Youku:     return new WbSearchEngine(Youku,    WBSE_YOUKU,    tr("Youku"),    WBRC_IMAGE_YOUKU,    QStringList("youku") << "优酷");
  case WikiJa:    return new WbSearchEngine(WikiJa,   WBSE_WIKI_JA,  tr("Wikipedia"),WBRC_IMAGE_WIKI,     QStringList("w") << "wiki" << "wikipedia" << "ja" << "ウィキ" << "百科");
  case WikiEn:    return new WbSearchEngine(WikiEn,   WBSE_WIKI_EN,  tr("Wikipedia"),WBRC_IMAGE_WIKI,     QStringList("en") << "wikipedia");
  case WikiZh:    return new WbSearchEngine(WikiZh,   WBSE_WIKI_ZH,  tr("Wikipedia"),WBRC_IMAGE_WIKI,     QStringList("zh") << "维基");
  case Qt:        return new WbSearchEngine(Qt,       WBSE_QT,       "Qt",           WBRC_IMAGE_QT,       QStringList("qt"));
  case CPlusPlus: return new WbSearchEngine(CPlusPlus,WBSE_CPP,      "C++",          WBRC_IMAGE_CPP,      QStringList("cpp") << "c++" << "cplusplus");
  case Bash:      return new WbSearchEngine(Bash,     WBSE_BASH,     "Bash",         WBRC_IMAGE_SS64,     QStringList("bash") << "sh" << "shell");
  case Cmd:       return new WbSearchEngine(Cmd,      WBSE_CMD,      "Cmd",          WBRC_IMAGE_SS64,     QStringList("cmd") << "dos" << "nt");
  case Mac:       return new WbSearchEngine(Mac,      WBSE_MAC,      "Mac",          WBRC_IMAGE_SS64,     QStringList("osx"));
  default: Q_ASSERT(0); return 0;
  }
}

// EOF
