#ifndef YAHOOJLPAPIS_P_H
#define YAHOOJLPAPIS_P_H

// 9/8/2012
// yahoojlpapis_p.h
// See: http://developer.yahoo.co.jp/webapi/jlp/

#define YAHOO_APPID CONFIG_YAHOO_ID

#define YAHOO_FURIGANA_API "http://jlp.yahooapis.jp/FuriganaService/V1/furigana"
#define YAHOO_MA_API "http://jlp.yahooapis.jp/MAService/V1/parse" // “ú–{ŒêŒ`‘Ô‘f‰ğÍAPI
#define YAHOO_DA_API "http://jlp.yahooapis.jp/DAService/V1/parse" // “ú–{ŒêŒW‚èó‚¯‰ğÍAP

#define YAHOO_QUERY_ID      "appid"
#define YAHOO_QUERY_TEXT    "sentence"

enum {
  H_Noun = 379694,  // "–¼Œ"
  H_Verb = 374702,  // "“®Œ"
  H_Special = 496410  // "“Áê"
};

#endif // YAHOOJLPAPIS_P_H
