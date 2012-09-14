#ifndef INFOSEEKTRANSLATOR_P_H
#define INFOSEEKTRANSLATOR_P_H

// infoseektranslator_p.h
// 8/20/2012
// See: http://transer.com/sdk/rest_api_function.html
// See (auth): http://translation.infoseek.ne.jp/js/translation-text.js
// See (lang): http://translation.infoseek.ne.jp/js/userinfo.js

// - Defines -

// See: CrossLicense in translation-text.jp
#define INFOSEEK_AUTH_USER "infoseek/main"
#define INFOSEEK_AUTH_KEY "e3f33620ae053e48cdba30a16b1084b5d69a3a6c"

// sent=false, equiv=false to skip posting json
#define INFOSEEK_API   "http://translation.infoseek.ne.jp/clsoap/translate?sent=false&equiv=false"
#define INFOSEEK_QUERY_AUTH  "key"
#define INFOSEEK_QUERY_LANG  "e"
#define INFOSEEK_QUERY_TEXT  "t"

#define INFOSEEK_AREA_BEGIN "\"text\":\""
#define INFOSEEK_AREA_END   "\"}]}"

#endif // INFOSEEKTRANSLATOR_P_H
