#ifndef KOTOBANKTRANSLATOR_P_H
#define KOTOBANKTRANSLATOR_P_H

// kotobanktranslator_p.h
// 8/20/2012

// - Defines -
#define KOTOBANK_SEARCH_API "http://kotobank.jp/search/result?q="
#define KOTOBANK_WORD_API   "http://kotobank.jp/word/"

#define KOTOBANK_SEARCH_BEGIN   "<div class=\"contentsIn\">"
#define KOTOBANK_SEARCH_END     "<!-- /contentsIn -->"
#define KOTOBANK_WORD_BEGIN     "<div id=\"word_box\">"
#define KOTOBANK_WORD_END       "<!--word_box-->"

#define KOTOBANK_SEARCH_REMOVE_SPONSOR  "<div class=\"sponsorLink\">.*<!-- /sponsorLink -->"

#define KOTOBANK_WORD_REMOVE_HEADER     "<ul id=\"word_information\">(?:(?!<ul class=\"word_dic\">).)*(?=<ul class=\"word_dic\">)"
#define KOTOBANK_WORD_REMOVE_FOOTER     "<div id=\"banner_app\".*(?=</div>$)"   // until the end
#define KOTOBANK_WORD_REMOVE_SEARCH     "<li class=\"word_open\">(?:(?!</li>).)*</li>"

#endif // KOTOBANKTRANSLATOR_P_H
