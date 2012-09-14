#ifndef ACSS_H
#define ACSS_H

// ac/acss.h
// 7/15/2011
// See: http://doc.qt.nokia.com/stable/stylesheet-examples.html
// See: http://doc.qt.nokia.com/4.7/stylesheet-reference.html

#include "src/common/acrc.h"
#include "lib/qtext/ss.h"
#include <QtGlobal>

// - Buttons -

// Tool buttons

#define ACSS_TOOLBUTTON_TEXT_(_weight, _style, _dec, \
                              _normal, _hover, _pressed, _checked, _checked_hover, _disabled) \
  SS_BEGIN(QToolButton) \
    SS_SEMI_TRANSPARENT \
    SS_FONT_WEIGHT(_weight) \
    SS_FONT_STYLE(_style) \
    SS_TEXT_DECORATION(_dec) \
    SS_COLOR(_normal) \
    SS_BORDER_RADIUS(5px) \
    SS_PADDING(3px) \
  SS_END \
  SS_BEGIN(QToolButton:hover) \
    SS_BACKGROUND_COLOR(rgba(173,216,230,200)) \
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

// lightblue:rgb(173,216,230)
#ifdef Q_OS_MAC
# define ACSS_TOOLBUTTON_TEXT            ACSS_TOOLBUTTON_TEXT_(normal, normal, none, blue, red, purple, purple, red, gray)
# define ACSS_TOOLBUTTON_TEXT_NORMAL     ACSS_TOOLBUTTON_TEXT_(normal, normal, none, snow, red, purple, purple, red, gray)
//# define ACSS_TOOLBUTTON_TEXT_INVERT     ACSS_TOOLBUTTON_TEXT_(normal, normal, none, red, snow, purple, purple, red, gray)
//# define ACSS_TOOLBUTTON_TEXT_HIGHLIGHT  ACSS_TOOLBUTTON_TEXT_(bold, normal, none, snow, red, purple, purple, red, gray)
# define ACSS_TOOLBUTTON_TEXT_CHECKABLE  ACSS_TOOLBUTTON_TEXT_(bold, normal, none, snow, red, purple, purple, red, gray)
//# define ACSS_TOOLBUTTON_TEXT_TAB        ACSS_TOOLBUTTON_TEXT_(bold, normal, none, snow, red, purple, purple, red, gray)
# define ACSS_TOOLBUTTON_TEXT_URL        ACSS_TOOLBUTTON_TEXT_(normal, italic, underline, snow, red, purple, purple, red, gray)
#else
# define ACSS_TOOLBUTTON_TEXT            ACSS_TOOLBUTTON_TEXT_(normal, normal, none, blue, red, orange, orange, red, gray)
# define ACSS_TOOLBUTTON_TEXT_NORMAL     ACSS_TOOLBUTTON_TEXT_(normal, normal, none, snow, red, orange, orange, red, gray)
//# define ACSS_TOOLBUTTON_TEXT_INVERT     ACSS_TOOLBUTTON_TEXT_(normal, normal, none, red, snow, orange, orange, red, gray)
//# define ACSS_TOOLBUTTON_TEXT_HIGHLIGHT  ACSS_TOOLBUTTON_TEXT_(bold, normal, none, snow, red, orange, orange, red, gray)
# define ACSS_TOOLBUTTON_TEXT_CHECKABLE  ACSS_TOOLBUTTON_TEXT_(normal, normal, none, snow, red, orange, orange, red, gray)
//# define ACSS_TOOLBUTTON_TEXT_TAB        ACSS_TOOLBUTTON_TEXT_(bold, normal, none, snow, red, orange, orange, red, gray)
# define ACSS_TOOLBUTTON_TEXT_URL        ACSS_TOOLBUTTON_TEXT_(normal, italic, underline, snow, red, orange, orange, red, gray)
#endif // Q_OS_MAC

// lightblue:rgb(173,216,230)
#define ACSS_TOOLBUTTON_TEXT_HIGHLIGHT \
  SS_BEGIN(QToolButton) \
    SS_FONT_WEIGHT(bold) \
    SS_COLOR(snow) \
    SS_BORDER_RADIUS(5px) \
    SS_PADDING(3px) \
    SS_BACKGROUND_COLOR(rgba(173,216,230,100)) \
  SS_END \
  SS_BEGIN(QToolButton:hover) \
    SS_BACKGROUND_COLOR(rgba(173,216,230,200)) \
    SS_COLOR(red) \
  SS_END \
  SS_BEGIN(QToolButton:pressed) \
    SS_COLOR(orange) \
  SS_END \
  SS_BEGIN(QToolButton:checked) \
    SS_COLOR(orange) \
  SS_END \
  SS_BEGIN(QToolButton:checked:hover) \
    SS_COLOR(red) \
  SS_END \
  SS_BEGIN(QToolButton:disabled) \
    SS_COLOR(gray) \
    SS_BACKGROUND_COLOR(rgba(173,216,230,50)) \
  SS_END

// lightblue:rgb(173,216,230)
#define ACSS_TOOLBUTTON_TEXT_TAB \
  SS_BEGIN(QToolButton) \
    SS_FONT_WEIGHT(bold) \
    SS_COLOR(snow) \
    SS_BORDER_RADIUS(7px) \
    SS_PADDING(3px) \
    SS_SEMI_TRANSPARENT \
  SS_END \
  SS_BEGIN(QToolButton:hover) \
    SS_BACKGROUND_COLOR(rgba(173,216,230,200)) \
    SS_COLOR(red) \
  SS_END \
  SS_BEGIN(QToolButton:pressed) \
    SS_COLOR(orange) \
  SS_END \
  SS_BEGIN(QToolButton:checked) \
    SS_BACKGROUND_COLOR(rgba(173,216,230,100)) \
    SS_COLOR(orange) \
  SS_END \
  SS_BEGIN(QToolButton:checked:hover) \
    SS_BACKGROUND_COLOR(rgba(173,216,230,200)) \
    SS_COLOR(red) \
  SS_END \
  SS_BEGIN(QToolButton:disabled) \
    SS_COLOR(gray) \
    SS_BACKGROUND_COLOR(rgba(173,216,230,50)) \
  SS_END

// orangered:rgb(255,69,0)
#define ACSS_TOOLBUTTON_TEXT_INVERT \
  SS_BEGIN(QToolButton) \
    SS_FONT_WEIGHT(bold) \
    SS_COLOR(snow) \
    SS_BORDER_RADIUS(5px) \
    SS_PADDING(5px) \
    SS_BACKGROUND_COLOR(rgba(255,69,0,100)) \
  SS_END \
  SS_BEGIN(QToolButton:hover) \
    SS_BACKGROUND_COLOR(rgba(255,69,0,150)) \
    SS_COLOR(red) \
  SS_END \
  SS_BEGIN(QToolButton:pressed) \
    SS_COLOR(orange) \
  SS_END \
  SS_BEGIN(QToolButton:checked) \
    SS_COLOR(orange) \
  SS_END \
  SS_BEGIN(QToolButton:checked:hover) \
    SS_COLOR(red) \
  SS_END \
  SS_BEGIN(QToolButton:disabled) \
    SS_COLOR(gray) \
    SS_BACKGROUND_COLOR(rgba(255,69,0,50)) \
  SS_END

// - Labels -
#define ACSS_LABEL_(_weight, _style, _dec, \
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
# define ACSS_LABEL            ACSS_LABEL_(normal, normal, none, snow, red, gray)
# define ACSS_LABEL_HIGHLIGHT  ACSS_LABEL_(bold, normal, none, snow, red, gray)
# define ACSS_LABEL_URL        ACSS_LABEL_(normal, italic, underline, snow, red, gray)
#else
# define ACSS_LABEL            ACSS_LABEL_(normal, normal, none, cyan, red, gray)
# define ACSS_LABEL_HIGHLIGHT  ACSS_LABEL_(bold, normal, none, snow, red, gray)
# define ACSS_LABEL_URL        ACSS_LABEL_(normal, italic, underline, snow, red, gray)
#endif // Q_OS_MAC

// - Lines -

#define ACSS_LINEEDIT \
  SS_BEGIN(QLineEdit) \
    SS_BORDER_IMAGE_URL(ACRC_IMAGE_LINEEDIT) \
    SS_BORDER(1px groove gray) \
    SS_BORDER_WIDTH(4px) \
    SS_BORDER_RADIUS(3px) \
    SS_COLOR(black) \
  SS_END

//#define ACSS_LINEEDIT_OSD
//  SS_BEGIN(QLineEdit)
//    SS_SEMI_TRANSPARENT
//    SS_BORDER(1px groove purple)
//    SS_COLOR(blue)
//  SS_END
//  SS_BEGIN(QLineEdit::hover)
//    SS_BORDER_IMAGE_URL(ACRC_IMAGE_LINEEDIT)
//    SS_BORDER(1px groove)
//    SS_BORDER_WIDTH(4px)
//    SS_COLOR(black)
//  SS_END

// See: http://doc.qt.nokia.com/stable/stylesheet-examples.html#customizing-qcombobox
// TODO: Change QAbstractItemView style as well!
#define ACSS_COMBOBOX \
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

//#define ACSS_COMBOBOX_OSD
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

// - Boxes -

#define ACSS_CHECKBOX_(_normal, _hover, _checked, _checked_hover, _disabled) \
  SS_BEGIN(QCheckBox) \
    SS_COLOR(_normal) \
  SS_END \
  SS_BEGIN(QCheckBox:hover) \
    SS_COLOR(_hover) \
  SS_END \
  SS_BEGIN(QCheckBox:checked) \
    SS_COLOR(_checked) \
  SS_END \
  SS_BEGIN(QCheckBox:checked:hover) \
    SS_COLOR(_checked_hover) \
  SS_END \
  SS_BEGIN(QCheckBox:disabled) \
    SS_COLOR(_disabled) \
  SS_END
#define ACSS_CHECKBOX ACSS_CHECKBOX_(snow, red, cyan, orange, gray)

#define ACSS_RADIOBUTTON_(_normal, _hover, _checked, _checked_hover, _disabled) \
  SS_BEGIN(QRadioButton) \
    SS_COLOR(_normal) \
  SS_END \
  SS_BEGIN(QRadioButton:hover) \
    SS_COLOR(_hover) \
  SS_END \
  SS_BEGIN(QRadioButton:checked) \
    SS_COLOR(_checked) \
  SS_END \
  SS_BEGIN(QRadioButton:checked:hover) \
    SS_COLOR(_checked_hover) \
  SS_END \
  SS_BEGIN(QRadioButton:disabled) \
    SS_COLOR(_disabled) \
  SS_END
#define ACSS_RADIOBUTTON  ACSS_RADIOBUTTON_(snow, red, cyan, cyan, gray)

#define ACSS_GROUPBOX \
  SS_BEGIN(QGroupBox) \
    SS_FONT_WEIGHT(bold) \
    SS_COLOR(orange) \
  SS_END \
  SS_BEGIN(QGroupBox::disabled) \
    SS_COLOR(gray) \
  SS_END

// - Item views -

#define ACSS_LISTVIEW \
  SS_BEGIN(QListView) \
    SS_BACKGROUND_IMAGE_URL(ACRC_IMAGE_BACKGROUND) \
    SS_BORDER_WIDTH(4px) \
    SS_BORDER_RADIUS(3px) \
  SS_END

#define ACSS_TABLEVIEW \
  SS_BEGIN(QTableView) \
    SS_BACKGROUND_IMAGE_URL(ACRC_IMAGE_BACKGROUND) \
    SS_BORDER_WIDTH(4px) \
    SS_BORDER_RADIUS(3px) \
  SS_END

#define ACSS_TREEVIEW \
  SS_BEGIN(QTreeView) \
    SS_BACKGROUND_IMAGE_URL(ACRC_IMAGE_BACKGROUND) \
    SS_BORDER_WIDTH(4px) \
    SS_BORDER_RADIUS(3px) \
  SS_END

// - Graphics views -

#define ACSS_GRAPHICSVIEW \
  SS_BEGIN(QGraphicsView) \
    SS_TRANSPARENT \
    SS_BORDERLESS \
  SS_END

// - Menus -
#define ACSS_MENU SS_NULL

// See: http://doc.qt.nokia.com/latest/stylesheet-examples.html#customizing-qmenu
// See: http://drpeterjones.com/colorcalc/
#define ACSS_CONTEXTMENU \
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

#define ACSS_TEXTEDIT \
  SS_BEGIN(QTextEdit) \
    SS_BORDER_IMAGE_URL(ACRC_IMAGE_TEXTEDIT) \
  SS_END

#define ACSS_TEXTBROWSER \
  SS_BEGIN(QTextBrowser) \
    SS_BORDER_IMAGE_URL(ACRC_IMAGE_TEXTBROWSER) \
  SS_END

// - Windows -

//#define ACSS_WINDOW
//  SS_BEGIN(QWidget)
//    SS_BACKGROUND_IMAGE_URL(ACRC_IMAGE_AERO)
//  SS_END
#define ACSS_WINDOW     SS_NULL

//#ifdef USE_WIN_DWM
//  #define ACSS_WINDOW_DWM
//    SS_BEGIN(QWidget)
//      SS_BORDER_RADIUS(8px)
//    SS_END
//#endif // USE_WIN_DWM
//
#define ACSS_WINDOW_DWM   SS_NULL

// - Status bar -

#define ACSS_STATUSBAR_DWM \
  SS_BEGIN(QStatusBar) \
    SS_COLOR(cyan) \
  SS_END

// - Customized -

#define ACSS_BACKGROUND_CLASS(_class) \
  SS_BEGIN(_class) \
    SS_BACKGROUND_IMAGE_URL(ACRC_IMAGE_AERO) \
  SS_END

#endif // ACSS_H
