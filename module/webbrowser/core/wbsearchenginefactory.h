#ifndef WBSEARCHENGINEFACTORY_H
#define WBSEARCHENGINEFACTORY_H

// core/wbsearchenginefactory.h
// 4/25/2012

#include "core/wbsearchengine.h"

#define WBSE_GOOGLE     "https://www.google.com/search?hl=ja&ie=UTF-8&q=%1"
#define WBSE_GOOGLE_IMAGES "https://www.google.com/search?hl=ja&q=%1"
#define WBSE_BING       "http://www.bing.com/search?q=%1"

#define WBSE_YOUTUBE    "http://www.youtube.com/results?search_query=%1"
#define WBSE_NICOVIDEO  "http://www.nicovideo.jp/search/%1"
#define WBSE_BILIBILI   "http://www.bilibili.tv/search?orderby=pubdate&keyword=%1@新番"
#define WBSE_YOUKU      "http://www.soku.com/search_video/q_%1"

#define WBSE_WIKI_JA    "http://ja.wikipedia.org/wiki/%1"
#define WBSE_WIKI_EN    "http://en.wikipedia.org/wiki/%1"
#define WBSE_WIKI_ZH    "http://zh.wikipedia.org/wiki/%1"

#define WBSE_MAN        "http://www.linuxmanpages.com/man1/%1.1.php"
#define WBSE_CPP        "http://cplusplus.com/search.do?q=%1"
#define WBSE_QT         "http://doc.qt.nokia.com/latest/%1.html"
#define WBSE_OSX        "https://developer.apple.com/library/mac/search/?q=%1"
#define WBSE_MSDN       "http://social.msdn.microsoft.com/search/en-us/windows/desktop?Refinement=181&query=%1"
#define WBSE_BASH       "http://ss64.com/bash/%1.html"
#define WBSE_CMD        "http://ss64.com/nt/%1.html"

///  Use object only to enable translation.
class WbSearchEngineFactory : public QObject
{
  Q_OBJECT
  typedef WbSearchEngineFactory Self;
  typedef QObject Base;

public:
  static Self *globalInstance() { static Self g; return &g; }
protected:
  explicit WbSearchEngineFactory(QObject *parent = 0) : Base(parent) { }

public:
  enum Engine { Google = 0, GoogleImages, Youtube, Nicovideo, Bilibili, Youku, WikiJa,
                WikiEn, WikiZh, Bing, Manpage, CPlusPlus, Qt, Osx, Msdn, Bash, Cmd,
                EngineCount, VisibleEngineCount = WikiJa +1
  };

  WbSearchEngine *create(int engine) const;
};

#endif // WBSEARCHENGINEFACTORY_H
