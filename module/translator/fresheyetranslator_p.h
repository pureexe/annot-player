#ifndef FRESHEYETRANSLATOR_P_H
#define FRESHEYETRANSLATOR_P_H

// fresheyetranslator_config.h
// 8/2l/2012
// See: https://github.com/alice0775/userChrome.js/blob/master/popupTranslate.uc.xul

// - Defines -

// See: http://ready.to/search/list/cs_trans.htm
// [both=T]で原文と訳文の両方を出力します。[both=F]で訳文のみを出力します。
#define FRESHEYE_API_EN     "http://mt.fresheye.com/ft_result.cgi"
#define FRESHEYE_API_ZH     "http://mt.fresheye.com/ft_cjresult.cgi"

#define FRESHEYE_POST_JA_EN "e=JE&gen_text="
#define FRESHEYE_POST_EN_JA "e=EJ&gen_text="
#define FRESHEYE_POST_JA_CN "cjjc=jc&charset=gb2312&gen_text="
#define FRESHEYE_POST_JA_TW "cjjc=jc&charset=big5&gen_text="
#define FRESHEYE_POST_CN_JA "cjjc=cj&charset=gb2312&gen_text="
#define FRESHEYE_POST_TW_JA "cjjc=cj&charset=big5&gen_text="

#define FRESHEYE_AREA_BEGIN     "<TEXTAREA class=\"out-form\""
#define FRESHEYE_AREA_BEGIN2    ">"
#define FRESHEYE_AREA_END       "</TEXTAREA>"

#endif // FRESHEYETRANSLATOR_P_H
