#ifndef WBSS_H
#define WBSS_H

// global/wbss.h
// 3/31/2012

#include "global/wbrc.h"
#include "htmlutil/sstags.h"
#include <QtGlobal>

// - Combo box -

#define SS_COMBOBOX_BASE \
  SS_BEGIN(QComboBox) \
    SS_BORDER_IMAGE_URL(WBRC_IMAGE_COMBOBOX) \
    SS_BORDER_WIDTH(4px) \
    SS_BORDER_RADIUS(3px) \
  SS_END \
  SS_BEGIN(QComboBox::drop-down) \
    SS_SUBCONTROL_ORIGIN_PADDING \
    SS_SUBCONTROL_POSITION(top left) \
    "border-top-left-radius:3px;" \
    "border-bottom-left-radius:3px;" \
  SS_END

// See: http://qt-project.org/doc/qt-4.8/stylesheet-examples.html
#define SS_COMBOBOX_NOICON \
  SS_COMBOBOX_BASE \
  SS_BEGIN(QComboBox::down-arrow) \
    SS_TRANSPARENT \
  SS_END

#define SS_COMBOBOX_ICON(_url) \
  SS_COMBOBOX_BASE \
  SS_BEGIN(QComboBox::down-arrow) \
    SS_IMAGE_URL(_url) \
  SS_END \
  SS_BEGIN(QComboBox::down-arrow:on) \
    SS_TOP(1px) \
    SS_LEFT(1px) \
  SS_END

// - Status bar -

#define SS_STATUSBAR_(_color, _weight) \
  SS_BEGIN(QStatusBar) \
    SS_COLOR(_color) \
    SS_FONT_WEIGHT(_weight) \
  SS_END
#define SS_STATUSBAR_WARNING    SS_STATUSBAR_(orange,normal)
#define SS_STATUSBAR_ERROR      SS_STATUSBAR_(red,bold)
#define SS_STATUSBAR_NOTIFY     SS_STATUSBAR_(red,normal)
#define SS_STATUSBAR_MESSAGE    SS_STATUSBAR_(snow,normal)

// - Tool button -

#define SS_TOOLBUTTON_NEWTAB \
  SS_BEGIN(QToolButton) \
    SS_BORDER_IMAGE_URL(WBRC_IMAGE_NEWTAB) \
    SS_BORDER(2px) \
  SS_END \
  SS_BEGIN(QToolButton:hover) \
    SS_BORDER_IMAGE_URL(WBRC_IMAGE_NEWTAB_HOVER) \
  SS_END \
  SS_BEGIN(QToolButton:pressed) \
    SS_BORDER_IMAGE_URL(WBRC_IMAGE_NEWTAB_PRESSED) \
  SS_END

#endif // WBSS_H
