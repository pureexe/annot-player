#ifndef WBSS_H
#define WBSS_H

// wbss.h
// 3/31/2012

#include "wbrc.h"
#include "module/qtext/ss.h"
#include <QtGlobal>

// - Combo box -

#define SS_COMBOBOX \
  SS_BEGIN(QComboBox) \
    SS_BORDER_IMAGE_URL(WBRC_IMAGE_COMBOBOX) \
    SS_BORDER_WIDTH(4px) \
    SS_BORDER_RADIUS(3px) \
  SS_END \
  SS_BEGIN(QComboBox::drop-down) \
    SS_TRANSPARENT \
  SS_END \
  SS_BEGIN(QComboBox::down-arrow) \
    SS_TRANSPARENT \
  SS_END \
  SS_BEGIN(QComboBox::down-arrow:pressed) \
    SS_TRANSPARENT \
  SS_END

// - Status bar -

#define SS_STATUSBAR_(_color) \
  SS_BEGIN(QStatusBar) \
    SS_COLOR(_color) \
  SS_END
#define SS_STATUSBAR_WARNING    SS_STATUSBAR_(orange)
#define SS_STATUSBAR_ERROR      SS_STATUSBAR_(red)
#define SS_STATUSBAR_NOTIFY     SS_STATUSBAR_(red)
#ifdef Q_WS_X11
#  define SS_STATUSBAR_MESSAGE  SS_STATUSBAR_(black)
#else
#  define SS_STATUSBAR_MESSAGE  SS_STATUSBAR_(cyan)
#endif // Q_WS_X11

#endif // WBSS_H
