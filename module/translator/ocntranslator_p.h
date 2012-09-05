#ifndef OCNTRANSLATOR_P_H
#define OCNTRANSLATOR_P_H

// ocntranslator_p.h
// 7/15/2012

// - Defines -
#define OCN_API      "http://cgi01.ocn.ne.jp/cgi-bin/translation/index.cgi"
#define OCN_KEY_LANG "langpair"
#define OCN_KEY_TEXT "sourceText"
#define OCN_KEY_AUTH "auth"

// See: http://www.hongfire.com/forum/showthread.php/94395-Translation-Aggregator-v0-4-9/page114
#define OCN_AUTH_API "http://cgi01.ocn.ne.jp/cgi-bin/translation/counter.cgi?prm=63676930312e6f636e2e6e652e6a70"
#define OCN_AUTH_BEGIN "'"
#define OCN_AUTH_END   "'"

#define OCN_AREA_BEGIN "<TEXTAREA NAME=\"responseText\""

#define OCN_AREA_BEGIN2 ">"
#define OCN_AREA_END   "</TEXTAREA>"

// - Authentification -

#define OCN_AUTH_PREFIX "0a15be08"
#define OCN_AUTH_SUFFIX "63676930312e6f636e2e6e652e6a70"

#define OCN_FORBIDDEN_TEXT  "Translation-Forbidden:\nYou don&#39;t have permission to access translation-system."

#endif // OCNTRANSLATOR_P_H
