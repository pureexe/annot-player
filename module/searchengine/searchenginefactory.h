#ifndef SEARCHENGINEFACTORY_H
#define SEARCHENGINEFACTORY_H

// searchenginefactory.h
// 4/25/2012

#include "module/searchengine/searchengine.h"

#define SE_GOOGLE     "https://www.google.com/search?hl=ja&ie=UTF-8&q=%1"
#define SE_GOOGLE_IMAGES "https://www.google.com/search?hl=ja&ie=UTF-8&tbm=isch&q=%1"
#define SE_BING       "http://www.bing.com/search?q=%1"

#define SE_YOUTUBE    "http://www.youtube.com/results?search_query=%1"
#define SE_NICOVIDEO  "http://www.nicovideo.jp/search/%1"
#define SE_BILIBILI   "http://www.bilibili.tv/search?orderby=pubdate&keyword=%1@新番"
#define SE_ACFUN      "http://www.acfun.tv/search.aspx?q=%1"
#define SE_YOUKU      "http://www.soku.com/search_video/q_%1"

#define SE_WIKI_JA    "http://ja.wikipedia.org/wiki/%1"
#define SE_WIKI_EN    "http://en.wikipedia.org/wiki/%1"
#define SE_WIKI_ZH    "http://zh.wikipedia.org/wiki/%1"

#define SE_MAN        "http://www.linuxmanpages.com/man1/%1.1.php"
#define SE_CPP        "http://cplusplus.com/search.do?q=%1"
#define SE_QT         "http://doc.qt.nokia.com/latest/%1.html"
#define SE_IP         "http://ip-address-lookup-v4.com/ip/%1"
#define SE_OSX        "https://developer.apple.com/library/mac/search/?q=%1"
#define SE_MSDN       "http://social.msdn.microsoft.com/search/en-us/windows/desktop?Refinement=181&query=%1"
#define SE_BASH       "http://ss64.com/bash/%1.html"
#define SE_CMD        "http://ss64.com/nt/%1.html"

///  Use object only to enable translation.
class SearchEngineFactory : public QObject
{
  Q_OBJECT
  Q_DISABLE_COPY(SearchEngineFactory)
  typedef SearchEngineFactory Self;
  typedef QObject Base;

public:
  static Self *globalInstance() { static Self g; return &g; }
protected:
  explicit SearchEngineFactory(QObject *parent = 0) : Base(parent) { }

public:
  enum Engine { Google = 0, GoogleImages, Bing,
                Youtube, Nicovideo, Bilibili, Acfun, Youku,
                WikiJa, WikiEn, WikiZh,
                Manpage, CPlusPlus, Qt, Ip, Osx, Msdn, Bash, Cmd,
                EngineCount, VisibleEngineCount = WikiZh +1
  };

  SearchEngine *create(int engine) const;
};

#endif // WBSEARCHENGINEFACTORY_H
