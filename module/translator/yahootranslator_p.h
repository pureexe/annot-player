#ifndef YAHOOTRANSLATOR_P_H
#define YAHOOTRANSLATOR_P_H

// yahootranslator_p.h
// 8/2l/2012
// See: https://github.com/alice0775/userChrome.js/blob/master/popupTranslate.uc.xul

// - Defines -

// See: http://ready.to/search/list/cs_trans.htm
// [both=T]で原文と訳文の両方を出力します。[both=F]で訳文のみを出力します。
#define YAHOO_API       "http://honyaku.yahoo.co.jp/transtext/?both=F"
#define YAHOO_KEY_LANG  "eid"
#define YAHOO_KEY_TEXT  "text"

#define YAHOO_AREA_BEGIN "<textarea id=\"trn_textText\""
#define YAHOO_AREA_BEGIN2 ">"
#define YAHOO_AREA_END   "</textarea>"

#endif // YAHOOTRANSLATOR_P_H
