#ifndef _AC_ACSS_H
#define _AC_ACSS_H

// ac/acss.h
// 7/15/2011
// See: http://doc.qt.nokia.com/stable/stylesheet-examples.html
// See: http://doc.qt.nokia.com/4.7/stylesheet-reference.html

#include "ac/acrc.h"
#include "module/qtext/ss.h"
#include <QtGlobal>

// - Sliders -

// See: http://blog.sina.com.cn/s/blog_791f544a0100s2ml.html
//#define SS_SLIDER "QSlider::groove:horizontal { border: 1px solid #bbb; background: white; height: 10px; border-radius: 4px; } QSlider::sub-page:horizontal { background: qlineargradient(x1: 0, y1: 0,    x2: 0, y2: 1, stop: 0 #66e, stop: 1 #bbf); background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1, stop: 0 #bbf, stop: 1 #55f); border: 1px solid #777; height: 10px; border-radius: 4px; } QSlider::add-page:horizontal { background: #fff; border: 1px solid #777; height: 10px; border-radius: 4px; } QSlider::handle:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #eee, stop:1 #ccc); border: 1px solid #777; width: 13px; margin-top: -2px; margin-bottom: -2px; border-radius: 4px; } QSlider::handle:horizontal:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #fff, stop:1 #ddd); border: 1px solid #444; border-radius: 4px; } QSlider::sub-page:horizontal:disabled { background: #bbb; border-color: #999; } QSlider::add-page:horizontal:disabled { background: #eee; border-color: #999; } QSlider::handle:horizontal:disabled { background: #eee; border: 1px solid #aaa; border-radius: 4px; } "
// - groove
// - handle
// - sub-page
// - add-page
#define SS_SLIDER \
  SS_BEGIN(QSlider::groove:horizontal) \
    SS_BORDER(1px) \
    SS_HEIGHT(2px) \
    SS_NO_MARGIN \
    SS_LEFT(5px) SS_RIGHT(5px) \
  SS_END \
  SS_BEGIN(QSlider::sub-page:horizontal) \
    SS_BACKGROUND( \
      qlineargradient(spread:pad, \
        x1:0, y1:1, x2:0, y2:0, \
        stop:0    rgba(27, 5, 27, 255), \
        stop:0.25 rgba(99, 20, 102, 255), \
        stop:0.5  rgba(154, 30, 158, 255), \
        stop:1    rgba(173, 57, 176, 255))) \
  SS_END \
  SS_BEGIN(QSlider::add-page:horizontal) \
    SS_BACKGROUND(#555) \
    SS_BORDER(1px solid #777) \
    SS_HEIGHT(1px) \
    SS_LEFT(5px) SS_RIGHT(5px) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal) \
    SS_TRANSPARENT \
    SS_MARGIN(-4px) \
    SS_WIDTH(8px) \
    SS_BORDER_RADIUS(4px) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal:hover) \
    SS_BACKGROUND( \
      qlineargradient( \
        x1:0, y1:0, x2:1, y2:1, \
        stop:0 #bbf, \
        stop:1 #66e)) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal:disabled) \
    SS_TRANSPARENT \
    SS_NO_MARGIN \
    SS_NO_WIDTH \
  SS_END \
  SS_BEGIN(QSlider::sub-page:horizontal:disabled) \
    SS_BACKGROUND(#bbb) \
    SS_BORDER_COLOR(#999) \
  SS_END \
  SS_BEGIN(QSlider::add-page:horizontal:disabled) \
    SS_BACKGROUND(#eee) \
    SS_BORDER_COLOR(#999) \
  SS_END \
  SS_BEGIN(QSlider::handle:horizontal:disabled) \
    SS_BACKGROUND(#eee) \
    SS_BORDER(1px solid #aaa) \
  SS_END

// - Buttons -

// Radio buttons

#define SS_RADIOBUTTON_TEXT_(_color) \
  SS_BEGIN(QRadioButton) \
    SS_TRANSPARENT \
    SS_COLOR(_color) \
  SS_END \

#define SS_RADIOBUTTON_TEXT     SS_RADIOBUTTON_TEXT_(blue)

// Tool buttons

#define SS_TOOLBUTTON_TEXT_(_weight, _style, _dec, \
                            _normal, _hover, _pressed, _checked, _checked_hover, _disabled) \
  SS_BEGIN(QToolButton) \
    SS_TRANSPARENT \
    SS_FONT_WEIGHT(_weight) \
    SS_FONT_STYLE(_style) \
    SS_TEXT_DECORATION(_dec) \
    SS_COLOR(_normal) \
  SS_END \
  SS_BEGIN(QToolButton:hover) \
    SS_COLOR(_hover) \
  SS_END \
  SS_BEGIN(QToolButton:pressed) \
    SS_COLOR(_pressed) \
  SS_END \
  SS_BEGIN(QToolButton:checked) \
    SS_COLOR(_checked) \
  SS_END \
  SS_BEGIN(QToolButton:checked:hover) \
    SS_COLOR(_checked_hover) \
  SS_END \
  SS_BEGIN(QToolButton:disabled) \
    SS_COLOR(_disabled) \
  SS_END

#ifdef Q_OS_MAC
  #define SS_TOOLBUTTON_TEXT_NORMAL       SS_TOOLBUTTON_TEXT_(normal, normal, none, blue, red, purple, purple, red, gray)
  #define SS_TOOLBUTTON_TEXT_INVERT       SS_TOOLBUTTON_TEXT_(normal, normal, none, red, blue, purple, purple, red, gray)
  #define SS_TOOLBUTTON_TEXT_HIGHLIGHT    SS_TOOLBUTTON_TEXT_(bold, normal, none, blue, red, purple, purple, red, gray)
  #define SS_TOOLBUTTON_TEXT_CHECKABLE    SS_TOOLBUTTON_TEXT_(bold, italic, none, blue, red, purple, purple, red, gray)
  #define SS_TOOLBUTTON_TEXT_TAB          SS_TOOLBUTTON_TEXT_(bold, normal, none, blue, red, purple, purple, red, gray)
  #define SS_TOOLBUTTON_TEXT_URL          SS_TOOLBUTTON_TEXT_(normal, italic, underline, blue, red, purple, purple, red, gray)
  #define SS_TOOLBUTTON_TEXT              SS_TOOLBUTTON_TEXT_NORMAL
#else
  #define SS_TOOLBUTTON_TEXT_NORMAL       SS_TOOLBUTTON_TEXT_(normal, normal, none, blue, red, orange, orange, red, gray)
  #define SS_TOOLBUTTON_TEXT_INVERT       SS_TOOLBUTTON_TEXT_(normal, normal, none, red, blue, orange, orange, red, gray)
  #define SS_TOOLBUTTON_TEXT_HIGHLIGHT    SS_TOOLBUTTON_TEXT_(bold, normal, none, blue, red, orange, orange, red, gray)
  #define SS_TOOLBUTTON_TEXT_CHECKABLE    SS_TOOLBUTTON_TEXT_(normal, italic, none, blue, red, orange, orange, red, gray)
  #define SS_TOOLBUTTON_TEXT_TAB          SS_TOOLBUTTON_TEXT_(bold, normal, none, blue, red, orange, orange, red, gray)
  #define SS_TOOLBUTTON_TEXT_URL          SS_TOOLBUTTON_TEXT_(normal, italic, underline, blue, red, orange, orange, red, gray)
  #define SS_TOOLBUTTON_TEXT              SS_TOOLBUTTON_TEXT_NORMAL
#endif // Q_OS_MAC

// - Labels -
#define SS_LABEL_(_weight, _style, _dec, \
                  _normal, _hover, _disabled) \
  SS_BEGIN(QLabel) \
    SS_FONT_WEIGHT(_weight) \
    SS_FONT_STYLE(_style) \
    SS_TEXT_DECORATION(_dec) \
    SS_COLOR(_normal) \
  SS_END \
  SS_BEGIN(QLabel:hover) \
    SS_COLOR(_hover) \
  SS_END \
  SS_BEGIN(QLabel:disabled) \
    SS_COLOR(_disabled) \
  SS_END

#ifdef Q_OS_MAC
  #define SS_LABEL           SS_LABEL_(normal, normal, none, blue, red, gray)
  #define SS_LABEL_HIGHLIGHT SS_LABEL_(normal, italic, none, purple, red, gray)
  #define SS_LABEL_URL       SS_LABEL_(normal, italic, underline, purple, red, gray)
#else
  #define SS_LABEL           SS_LABEL_(normal, normal, none, blue, red, gray)
  #define SS_LABEL_HIGHLIGHT SS_LABEL_(normal, italic, none, orange, red, gray)
  #define SS_LABEL_URL       SS_LABEL_(normal, italic, underline, orange, red, gray)
#endif // Q_OS_MAC

// - Lines -

#define SS_LINEEDIT \
  SS_BEGIN(QLineEdit) \
    SS_BORDER_IMAGE_URL(ACRC_IMAGE_LINEEDIT) \
    SS_BORDER(1px groove gray) \
    SS_BORDER_WIDTH(4px) \
    SS_BORDER_RADIUS(3px) \
    SS_COLOR(black) \
  SS_END

//#define SS_LINEEDIT_OSD
//  SS_BEGIN(QLineEdit)
//    SS_TRANSPARENT
//    SS_BORDER(1px groove purple)
//    SS_COLOR(blue)
//  SS_END
//  SS_BEGIN(QLineEdit::hover)
//    SS_BORDER_IMAGE_URL(ACRC_IMAGE_LINEEDIT)
//    SS_BORDER(1px groove)
//    SS_BORDER_WIDTH(4px)
//    SS_COLOR(black)
//  SS_END

// - Boxes -

// See: http://doc.qt.nokia.com/stable/stylesheet-examples.html#customizing-qcombobox
// TODO: Change QAbstractItemView style as well!
#define SS_COMBOBOX \
  SS_BEGIN(QComboBox) \
    SS_BORDER_IMAGE_URL(ACRC_IMAGE_BACKGROUND) \
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
  SS_END \
  SS_BEGIN(QComboBox QAbstractItemView) \
  SS_END

//#define SS_COMBOBOX_OSD
//  SS_LINEEDIT_OSD
//  SS_BEGIN(QComboBox)
//    SS_TRANSPARENT
//    SS_BORDER(1px groove purple)
//    SS_COLOR(blue)
//  SS_END
//  SS_BEGIN(QComboBox::drop-down)
//    SS_TRANSPARENT
//  SS_END
//  SS_BEGIN(QComboBox::down-arrow)
//    SS_TRANSPARENT
//  SS_END
//  SS_BEGIN(QComboBox QAbstractItemView)
//    SS_BORDER_IMAGE_URL(ACRC_IMAGE_BACKGROUND)
//  SS_END

// - Item views -

#define SS_TREEVIEW \
  SS_BEGIN(QTreeView) \
    SS_BACKGROUND_IMAGE_URL(ACRC_IMAGE_BACKGROUND) \
    SS_BORDER_WIDTH(4px) \
    SS_BORDER_RADIUS(3px) \
  SS_END

// - Graphics views -

#define SS_GRAPHICSVIEW \
  SS_BEGIN(QGraphicsView) \
    SS_TRANSPARENT \
    SS_BORDERLESS \
  SS_END

// - Menus -
#define SS_MENU SS_NULL

// See: http://doc.qt.nokia.com/latest/stylesheet-examples.html#customizing-qmenu
// See: http://drpeterjones.com/colorcalc/
#define SS_CONTEXTMENU \
  SS_BEGIN(QMenu::item) \
    SS_COLOR(orange) \
  SS_END \
  SS_BEGIN(QMenu::item::selected) \
    SS_BACKGROUND( \
      qlineargradient(spread:pad, \
        x1:0, y1:0, x2:1, y2:0, \
        stop:0    rgba(255, 204, 0, 64), \
        stop:0.25 rgba(255, 204, 51, 128), \
        stop:0.5  rgba(255, 204, 102, 64), \
        stop:1    rgba(255, 204, 153, 16))) \
  SS_END
//SS_BEGIN(QMenu)
//SS_BACKGROUND_IMAGE_URL(ACRC_IMAGE_CONTEXTMENU)
//SS_EN

// - Text edits -

#define SS_TEXTEDIT \
  SS_BEGIN(QTextEdit) \
    SS_BORDER_IMAGE_URL(ACRC_IMAGE_TEXTEDIT) \
  SS_END

// - Windows -

//#define SS_WINDOW
//  SS_BEGIN(QWidget)
//    SS_BACKGROUND_IMAGE_URL(ACRC_IMAGE_AERO)
//  SS_END
#define SS_WINDOW     SS_NULL

//#ifdef USE_WIN_DWM
//  #define SS_WINDOW_DWM
//    SS_BEGIN(QWidget)
//      SS_BORDER_RADIUS(8px)
//    SS_END
//#endif // USE_WIN_DWM
//
#define SS_WINDOW_DWM   SS_NULL


// - Status bar -

#define SS_STATUSBAR_DWM \
  SS_BEGIN(QStatusBar) \
    SS_COLOR(cyan) \
  SS_END

// - Customized -

#define SS_BACKGROUND_CLASS(_class) \
  SS_BEGIN(_class) \
    SS_BACKGROUND_IMAGE_URL(ACRC_IMAGE_AERO) \
  SS_END

#endif // _AC_ACSS_H
