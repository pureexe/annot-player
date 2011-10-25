#ifndef _CORE_HTMLTAG_H
#define _CORE_HTMLTAG_H

// core/htmltag.h
// 7/29/2011

#include <QString>

// - tag -

#define CORE_HTML_TAG_OPEN(_tag, ...)   "<" #_tag " " #__VA_ARGS__ ">"
#define CORE_HTML_TAG_CLOSE(_tag)       "</" #_tag ">"
#define CORE_HTML_TAG(_tag, ...)        "<" #_tag " " #__VA_ARGS__ "/>"

inline QString core_html_tag(const QString &tag, const QString &attr = "")
{ return "<" + tag + " " + attr + "/>"; }

inline QString core_html_tag_open(const QString &tag, const QString &attr = "")
{ return "<" + tag + " " + attr + ">"; }

inline QString core_html_tag_close(const QString &tag)
{ return "</" + tag + ">"; }

// - <br> -

#define CORE_HTML_BR_OPEN()             CORE_HTML_TAG_OPEN(br)
#define CORE_HTML_BR_CLOSE()            CORE_HTML_TAG_CLOSE(br)
#define CORE_HTML_BR()                  CORE_HTML_TAG(br)

inline QString core_html_br_open()      { return CORE_HTML_BR_OPEN(); }
inline QString core_html_br_close()     { return CORE_HTML_BR_CLOSE(); }
inline QString core_html_br()           { return CORE_HTML_BR(); }

// - <font> -
// Note: Font tag is deprecated since HTML 4.01, and removed in HTML 5

#define CORE_HTML_FONT_OPEN(_attr)      CORE_HTML_TAG_OPEN(font, _attr)
#define CORE_HTML_FONT_CLOSE()          CORE_HTML_TAG_CLOSE(font)
#define CORE_HTML_FONT(_qs, _attr)      (CORE_HTML_FONT_OPEN(_attr) + (_qs) + CORE_HTML_FONT_CLOSE())

inline QString core_html_font_open(const QString &attr = "")
{ return "<font " + attr + ">"; }

inline QString core_html_font_close() { return CORE_HTML_FONT_CLOSE(); }

inline QString core_html_font(const QString &content = "", const QString &attr = "")
{ return core_html_font_open(attr) + content + core_html_font_close(); }

// - <div> -

#define CORE_HTML_DIV_OPEN()            CORE_HTML_TAG_OPEN(div)
#define CORE_HTML_DIV_CLOSE()           CORE_HTML_TAG_CLOSE(div)
#define CORE_HTML_DIV(_qs)              (CORE_HTML_DIV_OPEN() + (_qs) + CORE_HTML_DIV_CLOSE())

inline QString core_html_div_open() { return CORE_HTML_DIV_OPEN(); }
inline QString core_html_div_close() { return CORE_HTML_DIV_CLOSE(); }

inline QString core_html_div(const QString &content = "")
{ return core_html_div_open() + content + core_html_div_close(); }

// - <span> -

#define CORE_HTML_SPAN_OPEN(_style)     CORE_HTML_TAG_OPEN(span, style=#_style)
#define CORE_HTML_SPAN_CLOSE()          CORE_HTML_TAG_CLOSE(span)
#define CORE_HTML_SPAN(_qs, _style)     (CORE_HTML_SPAN_OPEN(_style) + (_qs) + CORE_HTML_SPAN_CLOSE())

inline QString core_html_span_open(const QString &style = "")
{ return "<span style=\"" + style + "\">"; }

inline QString core_html_span_close() { return CORE_HTML_SPAN_CLOSE(); }

inline QString core_html_span(const QString &content = "", const QString &style = "")
{ return core_html_span_open(style) + content + core_html_span_close(); }


//#define CORE_HTML_DUMMY_OPEN()       CORE_HTML_DIV_OPEN(display:inline)
#define CORE_HTML_DUMMY_OPEN()       CORE_HTML_TAG_OPEN(span)
#define CORE_HTML_DUMMY_CLOSE()      CORE_HTML_TAG_CLOSE(span)
#define CORE_HTML_DUMMY(_qs)         (CORE_HTML_DUMMY_OPEN() + (_qs) + CORE_HTML_DUMMY_CLOSE())

inline QString core_html_dummy_open()   { return CORE_HTML_DUMMY_OPEN(); }
inline QString core_html_dummy_close()  { return CORE_HTML_DUMMY_CLOSE(); }

inline QString core_html_dummy(const QString &content)
{ return core_html_dummy_open() + content + core_html_dummy_close(); }

//#define HTML_STYLE_OPEN(_style) CORE_HTML_TAG_OPEN(div, style="display:inline;" #_style ) // Not working on Windows
//#define CORE_HTML_STYLE_OPEN(_style) "<div style=\"display:inline;" #_style "\">"
#define CORE_HTML_STYLE_OPEN(_style) CORE_HTML_TAG_OPEN(span, style=#_style)
#define CORE_HTML_STYLE_CLOSE()      CORE_HTML_TAG_CLOSE(span)
#define CORE_HTML_STYLE(_qs, _style) (CORE_HTML_STYLE_OPEN(_style) + (_qs) + CORE_HTML_STYLE_CLOSE())

inline QString core_html_style_open(const QString &style = "")
{ return "<span style=\"" + style + "\">"; }

inline QString core_html_style_close() { return CORE_HTML_STYLE_CLOSE(); }

inline QString core_html_style(const QString &content = "", const QString &style = "")
{ return core_html_style_open(style) + content + core_html_style_close(); }

#define CORE_HTML_EM_OPEN()          CORE_HTML_TAG_OPEN(em)
#define CORE_HTML_EM_CLOSE()         CORE_HTML_TAG_CLOSE(em)
#define CORE_HTML_EM(_qs)            (CORE_HTML_EM_OPEN() + (_qs) + CORE_HTML_EM_CLOSE())

inline QString core_html_em_open() { return CORE_HTML_EM_OPEN(); }
inline QString core_html_em_close() { return CORE_HTML_EM_CLOSE(); }

inline QString core_html_em(const QString &content = "")
{ return core_html_em_open() + content + core_html_em_close(); }

#define CORE_HTML_STRONG_OPEN()      CORE_HTML_TAG_OPEN(strong)
#define CORE_HTML_STRONG_CLOSE()     CORE_HTML_TAG_CLOSE(strong)
#define CORE_HTML_STRONG(_qs)        (CORE_HTML_STRONG_OPEN() + (_qs) + CORE_HTML_STRONG_CLOSE())

inline QString core_html_strong_open() { return CORE_HTML_STRONG_OPEN(); }
inline QString core_html_strong_close() { return CORE_HTML_STRONG_CLOSE(); }

inline QString core_html_strong(const QString &content = "")
{ return core_html_strong_open() + content + core_html_strong_close(); }


#endif // _CORE_HTMLTAG_H
