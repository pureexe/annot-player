#ifndef JDICTRANSLATOR_P_H
#define JDICTRANSLATOR_P_H

// jdictranslator_config.h
// 8/11/2012

// - Defines -
#define JDIC_API        "http://www.edrdg.org/cgi-bin/wwwjdic/wwwjdic?1E"
#define JDIC_POST_LANG  "dicsel"
#define JDIC_POST_TEXT  "dsrchkey"
#define JDIC_ENCODING   "EUC-JP"

#define JDIC_DOC_BEGIN  "<div style=\"clear: both;\">"
#define JDIC_DOC_END    "<div style=\"clear:left;\">"
#define JDIC_DOC_END2   "<input"

#define JDIC_REMOVE_RE \
  QRegExp("(?:" \
    "<div[^>]*>\n<INPUT [^>]*>\n</div>" "|" \
    "<INPUT [^>]*>" "|" \
    "<script>[^<]*</script>" \
  ")")

// <b>キーワード:</b>
// <INPUT TYPE="text"  NAME="dsrchkey" SIZE=30 VALUE="">&nbsp;&nbsp 辞書:
// <SELECT NAME="dicsel">
// <OPTION SELECTED VALUE="1" > Jpn-Eng General (EDICT)</OPTION>
// <OPTION VALUE="2" > Japanese Names (ENAMDICT)</OPTION>
// <OPTION VALUE="3" > Computing/Telecomms</OPTION>
// <OPTION VALUE="4" > Life Sciences/Bio-Med</OPTION>
// <OPTION VALUE="5" > Legal Terms</OPTION>
// <OPTION VALUE="6" > Finance/Marketing</OPTION>
// <OPTION VALUE="7" > Buddhism</OPTION>
// <OPTION VALUE="8" > Miscellaneous</OPTION>
// <OPTION VALUE="9" > Special Text-glossing</OPTION>
// <OPTION VALUE="A" > Engineering/Science</OPTION>
// <OPTION VALUE="B" > Linguistics</OPTION>
// <OPTION VALUE="C" > River & Water Systems</OPTION>
// <OPTION VALUE="D" > Automobile Industry</OPTION>
// <OPTION VALUE="E" > Japanese Wordnet</OPTION>
// <OPTION VALUE="F" > Work-in-progress File</OPTION>
// <OPTION VALUE="G" > Japanese-German (WaDoku)</OPTION>
// <OPTION VALUE="H" > Japanese-French</OPTION>
// <OPTION VALUE="I" > Japanese-Russian</OPTION>
// <OPTION VALUE="J" > Japanese-Swedish</OPTION>
// <OPTION VALUE="K" > Japanese-Hungarian</OPTION>
// <OPTION VALUE="L" > Japanese-Spanish</OPTION>
// <OPTION VALUE="M" > Japanese-Dutch</OPTION>
// <OPTION VALUE="N" > Japanese-Slovenian</OPTION>
// <OPTION VALUE="O" > Japanese-Italian</OPTION>
// <OPTION VALUE="P" > Untranslated</OPTION>
// <OPTION VALUE="Q" > Combined Jpn-Eng </OPTION>
// <OPTION VALUE="R" > Expanded Text-glossing</OPTION>
// </SELECT>

#define JDIC_DICT_EN   "1" // English
#define JDIC_DICT_DE   "G" // German
#define JDIC_DICT_FR   "H" // French
#define JDIC_DICT_RU   "I" // Russian
#define JDIC_DICT_ES   "L" // Spanish
#define JDIC_DICT_IT   "O" // Italian

#endif // JDICTRANSLATOR_P_H
