#ifndef _CORE_SS_H
#define _CORE_SS_H

// core/ss.h
// 7/15/2011
#include <QString>

// - QSS syntax -

#define SS_BEGIN(id)    #id "{"
#define SS_END          "}"

#define SS_NULL                 ";"
#define SS_PADDING(_len)        "padding:" #_len ";"
#define SS_NO_PADDING           SS_PADDING(0)
#define SS_TOP(_len)            "top:" #_len ";"
#define SS_BOTTOM(_len)         "bottom:" #_len ";"
#define SS_LEFT(_len)           "left:" #_len ";"
#define SS_RIGHT(_len)          "right:" #_len ";"
#define SS_IMAGE(_img)          "image:" #_img ";"
#define SS_IMAGE_URL(_url)      "image: url(" _url ");"
#define SS_BACKGROUND(_color)   "background:" #_color ";"
#define SS_BACKGROUND_COLOR(_color) "background-color:" #_color ";"
#define SS_BACKGROUND_IMAGE(_img)   "background-image:" #_img ";"
#define SS_TRANSPARENT          "background:transparent;"
#define SS_POSITION(_align)     "position:" #_align ";"
#define SS_POSITION_ABSOLUTE    SS_POSITION(absolute)
#define SS_POSITION_RELATIVE    SS_POSITION(relative)
#define SS_COLOR(_color)        "color:" #_color ";"
#define SS_BORDER_COLOR(_color) "border-color:" #_color ";"
#define SS_NO_BORDER            SS_BORDER(0)
#define SS_WIDTH(_len)          "width:" #_len ";"
#define SS_NO_WIDTH             SS_WIDTH(0)
#define SS_HEIGHT(_len)         "height:" #_len ";"
#define SS_NO_HEIGHT            SS_HEIGHT(0)
#define SS_BORDER(_len)         "border:" #_len ";"
#define SS_BORDER_RADIUS(_len)  "border-radius:" #_len ";"
#define SS_BORDER_WIDTH(_len)   "border-width:" #_len ";"
#define SS_BORDER_HEIGHT(_len)  "border-height:" #_len ";"
#define SS_BORDERLESS           "border-top:0;"
#define SS_MARGIN(_len)         "margin:" #_len ";"
#define SS_MARGIN_LEFT(_len)    "margin-left:" #_len ";"
#define SS_MARGIN_RIGHT(_len)   "margin-right:" #_len ";"
#define SS_MARGIN_TOP(_len)     "margin-top:" #_len ";"
#define SS_MARGIN_BOTTOM(_len)  "margin-bottom:" #_len ";"
#define SS_NO_MARGIN            SS_MARGIN(0)
#define SS_BORDER_IMAGE(_img)   "border-image:" #_img ";"
#define SS_BORDER_IMAGE_URL(_url) "border-image: url(" _url ");"
#define SS_SELECTION_COLOR(_color)"selection-color:" #_color ";"

#define SS_BACKGROUND_IMAGE_URL(_url) "background-image: url(" _url ");"
#define SS_BACKGROUND_IMAGE_URL_BEGIN "background-image: url("
#define SS_BACKGROUND_IMAGE_URL_END   ");"

#define SS_SUBCONTROL_POSITION(_align)  "subcontrol-position:" #_align ";"
#define SS_SUBCONTROL_ORIGIN(_origin) "subcontrol-origin:" #_origin ";"
#define SS_SUBCONTROL_ORIGIN_CONTENT    SS_SUBCONTROL_ORIGIN(content)
#define SS_SUBCONTROL_ORIGIN_PADDING    SS_SUBCONTROL_ORIGIN(padding)
#define SS_SUBCONTROL_ORIGIN_MARGIN     SS_SUBCONTROL_ORIGIN(margin)

inline QString
ss_background_image_url(const QString &url)
{ return SS_BACKGROUND_IMAGE_URL_BEGIN + url + SS_BACKGROUND_IMAGE_URL_END; }

#endif // _CORE_SS_H
