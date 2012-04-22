#ifndef _QTEXT_HTMLTAG_H
#define _QTEXT_HTMLTAG_H

// qtext/htmltag.h
// 7/29/2011

#include <QtCore/QString>

#define HTML_NULL       ""

// - tag -

#define HTML_TAG_OPEN(_tag, ...)   "<" #_tag " " #__VA_ARGS__ ">"
#define HTML_TAG_CLOSE(_tag)       "</" #_tag ">"
#define HTML_TAG(_tag, ...)        "<" #_tag " " #__VA_ARGS__ "/>"

inline QString html_tag(const QString &tag, const QString &attr = "")
{ return "<" + tag + " " + attr + "/>"; }

inline QString html_tag_open(const QString &tag, const QString &attr = "")
{ return "<" + tag + " " + attr + ">"; }

inline QString html_tag_close(const QString &tag)
{ return "</" + tag + ">"; }

// <html>

#define HTML_OPEN()             HTML_TAG_OPEN(html)
#define HTML_CLOSE()            HTML_TAG_CLOSE(html)

inline QString html_open()      { return HTML_OPEN(); }
inline QString html_close()     { return HTML_CLOSE(); }

// <header>

#define HTML_HEAD_OPEN()        HTML_TAG_OPEN(head)
#define HTML_HEAD_CLOSE()       HTML_TAG_CLOSE(head)
#define HTML_HEAD()             HTML_TAG(head)

inline QString html_head_open(const QString &attr = "")
{ return "<head " + attr + ">"; }
inline QString html_head_close(){ return HTML_HEAD_CLOSE(); }
inline QString html_head(const QString &content, const QString &attr = "")
{ return html_head_open(attr) + content + html_head_close(); }

// <body>

#define HTML_BODY_OPEN()        HTML_TAG_OPEN(body)
#define HTML_BODY_CLOSE()       HTML_TAG_CLOSE(body)
#define HTML_BODY()             HTML_TAG(body)

inline QString html_body_open(const QString &attr = "")
{ return "<body " + attr + ">"; }
inline QString html_body_close(){ return HTML_BODY_CLOSE(); }
inline QString html_body(const QString &content, const QString &attr = "")
{ return html_body_open(attr) + content + html_body_close(); }

// <title>

#define HTML_TITLE_OPEN()        HTML_TAG_OPEN(title)
#define HTML_TITLE_CLOSE()       HTML_TAG_CLOSE(title)
#define HTML_TITLE(_qs)          HTML_TITLE_OPEN() +(_qs)+ HTML_TITLE_CLOSE()

inline QString html_title_open() { return HTML_TITLE_OPEN(); }
inline QString html_title_close(){ return HTML_TITLE_CLOSE(); }
inline QString html_title(const QString &text)
{ return html_title_open() + text + html_title_close(); }

// <br>

#define HTML_BR_OPEN()          HTML_TAG_OPEN(br)
#define HTML_BR_CLOSE()         HTML_TAG_CLOSE(br)
#define HTML_BR()               HTML_TAG(br)

inline QString html_br_open()   { return HTML_BR_OPEN(); }
inline QString html_br_close()  { return HTML_BR_CLOSE(); }
inline QString html_br()        { return HTML_BR(); }

// <center>

#define HTML_CENTER_OPEN()             HTML_TAG_OPEN(center)
#define HTML_CENTER_CLOSE()            HTML_TAG_CLOSE(center)
#define HTML_CENTER()                  HTML_TAG(center)

inline QString html_center_open()      { return HTML_CENTER_OPEN(); }
inline QString html_center_close()     { return HTML_CENTER_CLOSE(); }
inline QString html_center()           { return HTML_CENTER(); }

// <font>
// Note: Font tag is deprecated since HTML 4.01, and removed in HTML 5

#define HTML_FONT_OPEN(_attr)      HTML_TAG_OPEN(font, _attr)
#define HTML_FONT_CLOSE()          HTML_TAG_CLOSE(font)
#define HTML_FONT(_qs, _attr)      HTML_FONT_OPEN(_attr) +(_qs)+ HTML_FONT_CLOSE()

inline QString html_font_open(const QString &attr = "")
{ return "<font " + attr + ">"; }

inline QString html_font_close() { return HTML_FONT_CLOSE(); }

inline QString html_font(const QString &content = "", const QString &attr = "")
{ return html_font_open(attr) + content + html_font_close(); }

// <a>

#define HTML_A_OPEN(_href)         "<a href=\"" _href "\">"
#define HTML_A_CLOSE()             HTML_TAG_CLOSE(a)
#define HTML_A(_qs, _href)         HTML_A_OPEN(_href) +(_qs)+ HTML_A_CLOSE()

inline QString html_a_open(const QString &href = "")
{ return HTML_A_OPEN(+ href +); }

inline QString html_a_close() { return HTML_A_CLOSE(); }

inline QString html_a(const QString &content = "", const QString &href = "")
{ return html_a_open(href) + content + html_a_close(); }

// <div>

#define HTML_DIV_OPEN()            HTML_TAG_OPEN(div)
#define HTML_DIV_CLOSE()           HTML_TAG_CLOSE(div)
#define HTML_DIV(_qs)              HTML_DIV_OPEN() +(_qs)+ HTML_DIV_CLOSE()

inline QString html_div_open() { return HTML_DIV_OPEN(); }
inline QString html_div_close() { return HTML_DIV_CLOSE(); }

inline QString html_div(const QString &content = "")
{ return html_div_open() + content + html_div_close(); }

// <span>

#define HTML_SPAN_OPEN(_style)     HTML_TAG_OPEN(span, style=#_style)
#define HTML_SPAN_CLOSE()          HTML_TAG_CLOSE(span)
#define HTML_SPAN(_qs, _style)     HTML_SPAN_OPEN(_style) +(_qs)+ HTML_SPAN_CLOSE()

inline QString html_span_open(const QString &style = "")
{ return "<span style=\"" + style + "\">"; }

inline QString html_span_close() { return HTML_SPAN_CLOSE(); }

inline QString html_span(const QString &content = "", const QString &style = "")
{ return html_span_open(style) + content + html_span_close(); }

//#define HTML_DUMMY_OPEN()       HTML_DIV_OPEN(display:inline)
#define HTML_DUMMY_OPEN()       HTML_TAG_OPEN(span)
#define HTML_DUMMY_CLOSE()      HTML_TAG_CLOSE(span)
#define HTML_DUMMY(_qs)         HTML_DUMMY_OPEN() +(_qs)+ HTML_DUMMY_CLOSE()

inline QString html_dummy_open()   { return HTML_DUMMY_OPEN(); }
inline QString html_dummy_close()  { return HTML_DUMMY_CLOSE(); }

inline QString html_dummy(const QString &content)
{ return html_dummy_open() + content + html_dummy_close(); }

//#define HTML_STYLE_OPEN(_style) HTML_TAG_OPEN(div, style="display:inline;" #_style ) // Not working on Windows
//#define HTML_STYLE_OPEN(_style) "<div style=\"display:inline;" #_style "\">"
#define HTML_STYLE_OPEN(_style) HTML_TAG_OPEN(span, style=#_style)
#define HTML_STYLE_CLOSE()      HTML_TAG_CLOSE(span)
#define HTML_STYLE(_qs, _style) HTML_STYLE_OPEN(_style) +(_qs)+ HTML_STYLE_CLOSE()

inline QString html_style_open(const QString &style = "")
{ return "<span style=\"" + style + "\">"; }

inline QString html_style_close() { return HTML_STYLE_CLOSE(); }

inline QString html_style(const QString &content = "", const QString &style = "")
{ return html_style_open(style) + content + html_style_close(); }

// <em>

#define HTML_EM_OPEN()          HTML_TAG_OPEN(em)
#define HTML_EM_CLOSE()         HTML_TAG_CLOSE(em)
#define HTML_EM(_qs)            HTML_EM_OPEN() +(_qs)+ HTML_EM_CLOSE()

inline QString html_em_open() { return HTML_EM_OPEN(); }
inline QString html_em_close() { return HTML_EM_CLOSE(); }

inline QString html_em(const QString &content = "")
{ return html_em_open() + content + html_em_close(); }

// <strong>

#define HTML_STRONG_OPEN()      HTML_TAG_OPEN(strong)
#define HTML_STRONG_CLOSE()     HTML_TAG_CLOSE(strong)
#define HTML_STRONG(_qs)        HTML_STRONG_OPEN() +(_qs)+ HTML_STRONG_CLOSE()

inline QString html_strong_open() { return HTML_STRONG_OPEN(); }
inline QString html_strong_close() { return HTML_STRONG_CLOSE(); }

inline QString html_strong(const QString &content = "")
{ return html_strong_open() + content + html_strong_close(); }

#endif // _QTEXT_HTMLTAG_H
